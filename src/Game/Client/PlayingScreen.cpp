#include "PlayingScreen.h"
#include "Game/GameConfig.h"

#include "Game/Client/Entity/Human.h"
#include "Game/Client/Entity/Zombie.h"
#include "Game/Client/Entity/Projectile.h"
#include "Game/Client/Entity/Cart.h"

#include "Game/NetObject/NetPlayerInfo.h"
#include "Game/NetObject/NetExplosion.h"

#include "Core/Client/ParticleEffects.h"
#include "Core/Client/Client.h"
#include "Core/Packer.h"
#include "Core/Protocol.h"
#include "Core/Logger.h"
#include "Core/Utility.h"

#include <iostream>

sf::Vector2f g_pos;

sf::Time SmoothClock::getElapsedTime()
{
	sf::Time dt = m_clock.getElapsedTime() - m_snap;

	if (dt >= m_converge)
		return m_target + dt;
	else
	{
		float progress = dt / m_converge;
		return m_current + (m_target + m_converge - m_current) * progress;
	}
}
void SmoothClock::reset(sf::Time target)
{
	m_clock.restart();
	m_snap = sf::Time::Zero;
	m_current = target;
	m_target = target;
	m_converge = sf::Time::Zero;
}

void SmoothClock::update(sf::Time target, sf::Time converge)
{
	m_current = getElapsedTime();
	m_target = target;
	m_snap = m_clock.getElapsedTime();

	sf::Time delta = target - m_current;
	if (delta < sf::Time::Zero && converge < -delta)
		converge = -delta;
	m_converge = converge;
}

PlayingScreen::PlayingScreen()
{
	m_entitiesByType.resize(static_cast<std::size_t>(EntityType::COUNT));
}

void PlayingScreen::onEnter(Client & client)
{
	Packer packer;
	packer.pack(Msg::CL_REQUEST_PLAYER_INFO);
	client.getNetwork().send(packer, true);

	Packer packer2;
	packer2.pack(Msg::CL_REQUEST_GAME_INFO);
	client.getNetwork().send(packer2, true);

	float verticleCameraSize = 2000.f;
	float horizontalCameraSize = verticleCameraSize / client.getWindow().getSize().x * client.getWindow().getSize().y;
	m_view.setSize(verticleCameraSize, horizontalCameraSize);
	
	sf::Vector2u windowSize = client.getWindow().getSize();

	m_font = client.getAssetManager().get<sf::Font>("assets/font/arial.ttf");
	m_predictionGraph = std::make_unique<Graph>(-150.f, 150.f, *m_font, "Prediction timing(ms)");
	m_predictionGraph->setPosition({ 0.f, 600. });
	m_snapshotGraph = std::make_unique<Graph>(-50.f, 100.f, *m_font, "Snapshot timing(ms)");
	m_snapshotGraph->setPosition({ 0.f, 300.f });


	m_particles.setTexture(*client.getAssetManager().get<sf::Texture>("assets/Untitled.png"));

	//ui
	m_editBox = tgui::EditBox::create();
	m_editBox->setPosition({ 0.f, client.getWindow().getSize().y - m_editBox->getSize().y });
	m_editBox->setSize("50%", m_editBox->getSize().y);
	m_editBox->hide();
	m_hud.reset(new Hud(*m_font, *this));

	auto sendChat = [&client, this]()
	{
		std::string chat = m_editBox->getText().toAnsiString();
		if (chat != "")
		{
			Packer packer;
			packer.pack(Msg::CL_CHAT);
			packer.pack(m_editBox->getText().toAnsiString());
			client.getNetwork().send(packer, true);
			m_editBox->setText("");
		}

	};
	m_editBox->onReturnKeyPress.connect(sendChat);
	client.getGui().add(m_editBox);
	

	m_chatBox = tgui::ChatBox::create();
	m_chatBox->getRenderer()->setBackgroundColor(sf::Color::Transparent);
	m_chatBox->getRenderer()->setBorderColor(sf::Color::Transparent);
	m_chatBox->setPosition({ 0.f, m_editBox->getPosition().y - m_chatBox->getSize().y });
	m_chatBox->setSize("50%", m_chatBox->getSize().y);
	client.getGui().add(m_chatBox);

	m_scoreBoard = tgui::Panel::create({ "&.height * .7", "80%" });
	m_scoreBoard->getRenderer()->setBackgroundColor(sf::Color(128, 128, 128, 50));
	m_scoreBoard->setPosition({ "&.width /2 - width / 2", "&.height / 2 - height / 2" });

	auto topPanel = tgui::Panel::create({ "100%", "10%" });
	topPanel->getRenderer()->setBackgroundColor(sf::Color::Green);
	topPanel->getRenderer()->setBorders(2);

		auto name = tgui::Label::create("Name");
		name->setSize("33%", "100%");
		name->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
		name->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
		topPanel->add(name);

		auto mode = tgui::Label::create("K/D/A");
		mode->setPosition({ tgui::bindRight(name), tgui::bindTop(name) });
		mode->setSize({ "33%", "100%" });
		mode->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
		mode->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
		topPanel->add(mode);

		auto status = tgui::Label::create("Ping");
		status->setPosition({ tgui::bindRight(mode), tgui::bindTop(mode) });
		status->setSize({ "33%", "100%" });
		status->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
		status->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
		topPanel->add(status);
	m_scoreBoard->add(topPanel);

	auto bottomPanel = tgui::ScrollablePanel::create({ "100%", "90% - 2" });
	bottomPanel->setPosition({ "0%", "10% + 2" });
	bottomPanel->getRenderer()->setBorders(2);

	auto grid = tgui::Grid::create();
	bottomPanel->add(grid, "grid");
	m_scoreBoard->add(bottomPanel, "bottomPanel");

	client.getGui().add(m_scoreBoard);
	m_scoreBoard->hide();
}

void PlayingScreen::handleEvent(const sf::Event & event, Client & client)
{
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab)
	{
		m_scoreBoard->show();
	}
	else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Tab)
	{
		m_scoreBoard->hide();
	}

	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Z)
	{
		g_pos = client.getWindow().mapPixelToCoords(sf::Mouse::getPosition(client.getWindow()), m_view);
	}
}

void PlayingScreen::handleNetEvent(ENetEvent & netEv, Client & client)
{
	if (netEv.type == ENET_EVENT_TYPE_RECEIVE)
	{
		Unpacker unpacker(netEv.packet->data, netEv.packet->dataLength);

		Msg msg;
		unpacker.unpack(msg);

		if (msg == Msg::SV_PLAYER_INFO)
		{
			std::size_t numPlayers;
			
			unpacker.unpack(numPlayers);
			unpacker.unpack(m_myPlayerId);
			std::cout << "total " << numPlayers << " players\n";
			std::cout << "my player id: " << m_myPlayerId << "\n";

			for (std::size_t i = 0; i < numPlayers; ++i)
			{
				PlayerInfo info;
				unpacker.unpack(info.id);
				unpacker.unpack(info.name);
				m_players.push_back(info);
				std::cout << info.id << ": " << info.name << "\n";
			}
		}

		else if (msg == Msg::SV_GAME_INFO)
		{
			std::string mapName;
			unpacker.unpack(mapName);

			//load map
			m_map.loadFromFile("map/" + mapName + ".xml");
			m_tileTexture = client.getAssetManager().get<sf::Texture>(m_map.getTilesetFile());
			m_tileTexture->setSmooth(true);

			//create tile map
			int tileSize = m_map.getTileSize();
			m_tileVertices.setPrimitiveType(sf::PrimitiveType::Quads);
			int textureWidth = m_tileTexture->getSize().x / tileSize;
			int textureHeight = m_tileTexture->getSize().y / tileSize;
			for (int y = 0; y < m_map.getSize().y; ++y)
			{
				for (int x = 0; x < m_map.getSize().x; ++x)
				{
					int tile = m_map.getTile(x, y);
					if (!m_map.getTile(x, y))
						continue;
					sf::Vertex a, b, c, d;
					a.position = sf::Vector2f(static_cast<float>(x * tileSize), static_cast<float>(y * tileSize));
					b.position = sf::Vector2f(static_cast<float>((x + 1) * tileSize), static_cast<float>(y * tileSize));
					c.position = sf::Vector2f(static_cast<float>((x + 1) * tileSize), static_cast<float>((y + 1) * tileSize));
					d.position = sf::Vector2f(static_cast<float>(x * tileSize), static_cast<float>((y + 1) * tileSize));
					int tx = (tile - 1) % textureWidth;
					int ty = (tile - 1) / textureHeight;

					a.texCoords = sf::Vector2f(static_cast<float>(tx * tileSize), static_cast<float>(ty * tileSize)) + sf::Vector2f(0.5f, 0.5f);
					b.texCoords = sf::Vector2f(static_cast<float>((tx + 1) * tileSize), static_cast<float>(ty * tileSize)) + sf::Vector2f(-0.5f, 0.5f);
					c.texCoords = sf::Vector2f(static_cast<float>((tx + 1) * tileSize), static_cast<float>((ty + 1) * tileSize)) + sf::Vector2f(-0.5f, -0.5f);
					d.texCoords = sf::Vector2f(static_cast<float>(tx * tileSize), static_cast<float>((ty + 1) * tileSize)) + sf::Vector2f(0.5f, -0.5f);

					m_tileVertices.append(a);
					m_tileVertices.append(b);
					m_tileVertices.append(c);
					m_tileVertices.append(d);
				}
			}

			//send ack
			Packer packer;
			packer.pack(Msg::CL_LOAD_COMPLETE);
			client.getNetwork().send(packer, true);
			Logger::getInstance().info("PlayingScreen", "Loading complete. Entering game...");
			m_state = ENTERING;
		}

		else if (msg == Msg::SV_FULL_SNAPSHOT || msg == Msg::SV_DELTA_SNAPSHOT)
		{
			if (m_state == LOADING)
				return;
			

			int serverTick;
			unpacker.unpack(serverTick);
			if (m_lastRecvTick >= serverTick)
				return;


			if (msg == Msg::SV_DELTA_SNAPSHOT)
			{
				int deltaTick;
				unpacker.unpack(deltaTick);
				
				const Snapshot * deltaSnapshot = m_snapshots.get(deltaTick);
				if (!deltaSnapshot)
				{
					m_lastRecvTick = -1;
					return;
				}
				Snapshot * snapshot = new Snapshot;
				snapshot->readRelativeTo(unpacker, *deltaSnapshot);
				m_snapshots.add(snapshot, serverTick);

			
			}
			else
			{
				Snapshot * snapshot = new Snapshot;
				snapshot->read(unpacker);
				m_snapshots.add(snapshot, serverTick);
			}
			m_lastRecvTick = serverTick;


			if (m_state == ENTERING)
			{
				if (m_numReceivedSnapshots == 0)
				{
					m_startTick = serverTick;
					m_predictedTime.reset(sf::seconds(static_cast<float>(m_startTick + 5) / TICKS_PER_SEC));
					m_prevPredictedTime = sf::seconds(static_cast<float>(m_startTick + 5) / TICKS_PER_SEC);
					m_predictedTick = m_startTick + 5;

				}
				else if(serverTick - 2 >= m_startTick) 
				{
					m_state = IN_GAME;
					m_renderTime.reset(sf::seconds(m_startTick / TICKS_PER_SEC));
				}
			}
			else if(m_state == IN_GAME)
			{
				sf::Time target = sf::seconds((serverTick - 2) / TICKS_PER_SEC);
				sf::Time timeLeft = sf::seconds(serverTick / TICKS_PER_SEC) - m_renderTime.getElapsedTime();
				m_renderTime.update(target, sf::seconds(1.f));
				m_repredict = true;

				m_snapshotGraph->addSample(timeLeft.asMicroseconds() / 1000.f);
			}
			m_numReceivedSnapshots++;
		}

		else if (msg == Msg::SV_INPUT_TIMING)
		{
			int inputTick;
			int32_t timeLeft;
			unpacker.unpack(inputTick);
			unpacker.unpack(timeLeft);
			for (const auto & input : m_inputs)
			{
				if (input.tick == inputTick)
				{
					sf::Time target = input.predictedTime + input.elapsed.getElapsedTime() - sf::milliseconds(timeLeft - 50);
					m_predictedTime.update(target, sf::seconds(1.f));
					m_predictionGraph->addSample(static_cast<float>(timeLeft));
				}
			}
		} 

		else if (msg == Msg::SV_ROUND_OVER)
		{
			Team winner;
			unpacker.unpack(winner);
			if (winner == Team::NONE)
				std::cout << "DRAW!\n";
			else if (winner == Team::A)
				std::cout << "TEAM A WIN\n";
			else if (winner == Team::B)
				std::cout << "TEAM B WIN\n";

			client.getScreenStack().pop();
		}

		else if (msg == Msg::SV_CHAT)
		{
			int tick;
			int id;
			std::string msg;
			unpacker.unpack(tick);
			unpacker.unpack(id);
			unpacker.unpack(msg);

			const PlayerInfo * info = getPlayerInfo(int(id));

			if (info)
			{
				float s = tick / TICKS_PER_SEC;
				std::string time = getStringFromTime(static_cast<int>(s));
				std::string line = "[" + time + "] " + info->name + ": " + msg;
				Logger::getInstance().info("Chat", line);
				m_chatBox->addLine(line);
			}

		}

		else if (msg == Msg::SV_PLAYER_LEFT)
		{
			int id;
			unpacker.unpack(id);
			const PlayerInfo * info = getPlayerInfo(id);
			if (info)
			{
				std::string line = info->name + " has left the game.";
				m_chatBox->addLine(line, sf::Color::Red);
			
				//m_players.erase(std::remove_if(m_players.begin(), m_players.end(), [id](const PlayerInfo & info) {return info.id == id; }), m_players.end());
			}
		}

		else if (msg == Msg::SV_KILL_FEED)
		{
			int killedPeerId, killerPeerId;
			unpacker.unpack(killedPeerId);
			unpacker.unpack(killerPeerId);


			const PlayerInfo * killedPeerInfo = getPlayerInfo(killedPeerId);
			const PlayerInfo * killerPeerInfo = getPlayerInfo(killerPeerId);
			
			std::string killedPeerName = killedPeerInfo ? killedPeerInfo->name : "Unknown player";
			std::string killerPeerName = killerPeerInfo ? killerPeerInfo->name : "Unknown player";
			std::string str = killerPeerName + " killed " + killedPeerName;
			//m_announcer.announce(str);
			m_hud->announce(str);
			m_chatBox->addLine(str, sf::Color::Red);
		}
	}

	else if (netEv.type == ENET_EVENT_TYPE_CONNECT)
	{

	}

	else if (netEv.type == ENET_EVENT_TYPE_DISCONNECT)
	{
	}
}

void PlayingScreen::handleUdpPacket(Unpacker & unpacker, const ENetAddress & addr, Client & client)
{

}

void PlayingScreen::update(Client & client)
{
	if(client.getInput().getKeyState(sf::Keyboard::Return, true))
	{
		if (m_editBox->isVisible())
		{
			m_editBox->hide();
		}
		else
		{
			m_editBox->show();
			m_editBox->focus();
		}
	}

	if (m_state == IN_GAME)
	{
		static sf::Clock clock;
		sf::Time current = m_predictedTime.getElapsedTime();
		sf::Time dt = current - m_prevPredictedTime;
		m_prevPredictedTime = current;
		m_accumulator += dt;


		int i = 0;
		while (m_accumulator >= sf::seconds(1.f / TICKS_PER_SEC))
		{
			++i;
			m_accumulator -= sf::seconds(1 / TICKS_PER_SEC);

			m_predictedTick++;


			//read input
			NetInput input;
			if(!m_editBox->isFocused())
				input = client.getInput().getInput(client.getWindow(), m_view);
		
			//save the input
			m_inputs[m_currentInputIndex].tick = m_predictedTick;
			m_inputs[m_currentInputIndex].input = input;
			m_inputs[m_currentInputIndex].predictedTime = current;
			m_inputs[m_currentInputIndex].elapsed.restart();
			m_currentInputIndex++;
			m_currentInputIndex = m_currentInputIndex % 200;

			//send to server
			Packer packer;
			packer.pack(Msg::CL_INPUT);
			packer.pack(m_predictedTick);
			packer.pack(m_lastRecvTick);
			input.write(packer);

			client.getNetwork().send(packer, false);
			client.getNetwork().flush();


			//TODO: improve this clusterfuck of logic

			const PlayerInfo * myPlayer = getPlayerInfo(m_myPlayerId);

			//Prediction
			for (auto p : m_predictedEntities)
			{
				p->tick(sf::seconds(1.f / TICKS_PER_SEC).asSeconds(), input, m_map);
				
			}
			

			if (m_repredict)
			{
				Snapshot * s = m_snapshots.getLast();

				for (auto e : m_predictedEntities)
				{
					if (e->rollback(*s))
					{
						for (int t = m_lastRecvTick + 1; t <= m_predictedTick; ++t)
						{
							NetInput input;
							for (const auto & i : m_inputs)
								if (i.tick == t)
									input = i.input;

							e->tick(sf::seconds(1.f / TICKS_PER_SEC).asSeconds(), input, m_map);

						}
					}
					
					m_repredict = false;
				}
			}

		}
		if (i > 1)
		{

			//std::cout << "double update!";
		}
		sf::Time timeSinceLastUpdate = clock.restart();
		m_particles.update(timeSinceLastUpdate.asSeconds());
		m_hud->update(timeSinceLastUpdate.asSeconds());
	}
}

void PlayingScreen::render(Client & client)
{
	if (m_state != IN_GAME)
		return;
	//find snapshots
	sf::Time currentRenderTime = m_renderTime.getElapsedTime();
	float renderTick = currentRenderTime.asSeconds() * TICKS_PER_SEC;

	const auto & s = m_snapshots.find(renderTick);

	SnapInfo prevSnap;
	prevSnap.tick = m_currentSnap.tick;
	prevSnap.snapshot = m_currentSnap.snapshot;

	m_currentSnap.tick = s.first->tick;
	m_currentSnap.snapshot = s.first->snapshot.get();

	m_nextSnap.tick = s.second ? s.second->tick : -1;
	m_nextSnap.snapshot = s.second ? s.second->snapshot.get() : nullptr;

	
	//calculate interp
	float t = 0.f;
	float predT = m_accumulator / sf::seconds(1.f / TICKS_PER_SEC);
	if (m_nextSnap.snapshot)
		t = (renderTick - s.first->tick) / (s.second->tick - s.first->tick);
	
	
	//transit snapshot
	if (prevSnap.tick != m_currentSnap.tick)
	{
		//create entities
		for (auto & p : m_currentSnap.snapshot->getEntities())
		{
			if (p.second->getType() == NetObject::PLAYER_INFO)
			{
				PlayerInfo * info = getPlayerInfo(p.first.id);
				NetPlayerInfo * netInfo = reinterpret_cast<NetPlayerInfo*>(p.second->data.data());
				if (info)
				{
					info->entityId = netInfo->id;
					info->team = netInfo->team;
					info->score = netInfo->score;
					info->ping = netInfo->ping;
					info->kills = netInfo->kills;
					info->deaths = netInfo->deaths;
					info->assists = netInfo->assists;
					info->respawnTick = netInfo->respawnTick;
				}

			}
			else
			{
				Entity * e = getEntity(p.first.id);
				if (!e)
				{
					switch (p.second->getType())
					{
					case NetObject::HUMAN:
						e = new Human(p.first.id, client, *this);
						break;
					case NetObject::ZOMBIE:
						e = new Zombie(p.first.id, client, *this);
						break;
					case NetObject::PROJECTILE:
						e = new Projectile(p.first.id, client, *this);
						break;
					case NetObject::CART:
						e = new Cart(p.first.id, client, *this);
					default:
						break;
					}
					m_entitiesByType[static_cast<int>(e->getType())].emplace_back(e);



					if (getPlayerInfo(m_myPlayerId) && p.first.id == getPlayerInfo(m_myPlayerId)->entityId)
					{
						PredictedEntity * p = static_cast<PredictedEntity*>(e);
						p->setPrediction(true);
						m_predictedEntities.push_back(p);
					}
				}
			}

			
		}
		//handle transient entities
		for (auto & p : m_currentSnap.snapshot->getTransients())
		{
			if (p->getType() == NetObject::EXPLOSION)
			{
				NetExplosion * ne = reinterpret_cast<NetExplosion*>(p->data.data());
				createExplosion(m_particles, static_cast<sf::Vector2f>(ne->pos) / 100.f);
			}
		}


		//delete entities
		for (auto & v : m_entitiesByType)
		{
			for (auto & e : v)
			{
				if (!e->find(*m_currentSnap.snapshot))
					e->setAlive(false);

			}
		}

		auto isDead2 = [](Entity * e) {return !e->isAlive(); };
		m_predictedEntities.erase(std::remove_if(m_predictedEntities.begin(), m_predictedEntities.end(), isDead2), m_predictedEntities.end());
		auto isDead = [](std::unique_ptr<Entity> & e) {return !e->isAlive(); };
		for (auto & v : m_entitiesByType)
			v.erase(std::remove_if(v.begin(), v.end(), isDead), v.end());

		m_snapshots.removeUntil(prevSnap.tick);


		//update scoreboard
		updateScoreboard();
	}

	//camera
	const PlayerInfo * myInfo = getPlayerInfo(m_myPlayerId);
	Entity * myEntity = getEntity(myInfo->entityId);
	if (myEntity)
	{
		sf::Vector2f center = myEntity->getCameraPosition(m_currentSnap.snapshot, m_nextSnap.snapshot, predT, t);
		sf::FloatRect area{ center - m_view.getSize() / 2.f, m_view.getSize() };
		sf::Vector2f worldSize = static_cast<sf::Vector2f>(m_map.getSize()) * static_cast<float>(m_map.getTileSize());
		if (area.left < 0.f)
		{
			center.x = m_view.getSize().x / 2.f;
		}
		else if (area.left + area.width > worldSize.x)
		{
			center.x = worldSize.x - m_view.getSize().x / 2.f;
		}
		if (area.top < 0.f)
		{
			center.y = m_view.getSize().y / 2.f;
		}
		else if (area.top + area.height > worldSize.y)
		{
			center.y = worldSize.y - m_view.getSize().y / 2.f;
		}
		m_view.setCenter(center);
	}
	else
	{
		std::cout << "Respawning in: " << myInfo->respawnTick  << "\n";
	}
	sf::RenderWindow & window = client.getWindow();
	window.setView(m_view);
	
	//draw background
	sf::RectangleShape background;
	background.setSize(static_cast<sf::Vector2f>(m_map.getSize() * m_map.getTileSize()));
	background.setFillColor(sf::Color(135, 206, 235));
	window.draw(background);

	//draw tile map
	sf::RenderStates states;
	states.texture = m_tileTexture;
	window.draw(m_tileVertices, states);

	//draw entities
	for (auto & v : m_entitiesByType)
	{
		for (auto & e : v)
		{
			e->render(m_currentSnap.snapshot, m_nextSnap.snapshot, predT, t);
		}
	}


	//particles
	window.draw(m_particles);

	
	if(client.debugRenderEnabled())
		debugRender(client, m_view);

	m_hud->setGameTime(currentRenderTime);
	m_hud->setEntity(myEntity);
	window.draw(*m_hud);
}

void PlayingScreen::onExit(Client & client)
{
}

void PlayingScreen::onObscure(Client & client)
{
}

void PlayingScreen::onReveal(Client & client)
{

}

Entity * PlayingScreen::getEntity(int id)
{
	for (auto & v : m_entitiesByType)
		for (auto & e : v)
			if (e->getId() == id)
				return e.get();
	return nullptr;
}
/*
Entity * PlayingScreen::getEntity(NetObject::Type type, int id)
{
	for (auto & e : m_entitiesByType[static_cast<int>(type)])
		if (e->getId() == id)
			return e.get();
	return nullptr;
}*/

void PlayingScreen::debugRender(Client & client, const sf::View & playerView)
{
	//draw grid
	sf::RenderWindow & window = client.getWindow();
	window.setView(playerView);
	sf::VertexArray arr;
	arr.setPrimitiveType(sf::Lines);
	for (int i = 0; i < m_map.getSize().x; ++i)
	{
		sf::Vertex v, v2;
		v.color = sf::Color::Black;
		v2.color = sf::Color::Black;
		v.position = sf::Vector2f(static_cast<float>(i * m_map.getTileSize()), 0.f);
		v2.position = sf::Vector2f(static_cast<float>(i * m_map.getTileSize()), static_cast<float>(m_map.getTileSize() * m_map.getSize().y));
		arr.append(v);
		arr.append(v2);
	}
	for (int i = 0; i < m_map.getSize().y; ++i)
	{
		sf::Vertex v, v2;
		v.color = sf::Color::Black;
		v2.color = sf::Color::Black;
		v.position = sf::Vector2f(0.f, static_cast<float>(i * m_map.getTileSize()));
		v2.position = sf::Vector2f(static_cast<float>(m_map.getSize().x * m_map.getTileSize()), static_cast<float>(i * m_map.getTileSize()));
		arr.append(v);
		arr.append(v2);
	}
	window.draw(arr);

	//draw mouse world pos
	sf::Vector2f mouseWorldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window), playerView);
	sf::Text text;
	text.setFont(*client.getAssetManager().get<sf::Font>("assets/font/arial.ttf"));
	int xt = static_cast<int>(std::floor(mouseWorldPos.x / m_map.getTileSize()));
	int yt = static_cast<int>(std::floor(mouseWorldPos.y / m_map.getTileSize()));
	std::string str = "(" + std::to_string(xt) + ", " + std::to_string(yt) + ")\n";
	str += "(" + std::to_string(mouseWorldPos.x) + ", " + std::to_string(mouseWorldPos.y) + ")";
	text.setString(str);
	text.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)));

	sf::VertexArray arr2{ sf::PrimitiveType::Lines, 2 };
	sf::Vertex v0;
	v0.position = g_pos;
	v0.color = sf::Color::Yellow;
	sf::Vertex v1;
	v1.position = mouseWorldPos;
	v1.color = sf::Color::Yellow;
	arr2.append(v0);
	arr2.append(v1);
	window.draw(arr2);

	float t;
	if (m_map.sweepPoints({ v0.position }, v1.position - v0.position, t))
	{
		sf::Vector2f intersectionPoint = v0.position + (v1.position - v0.position) * t;
		sf::CircleShape c;
		c.setRadius(10.f);
		c.setOrigin(10.f, 10.f);
		c.setPosition(intersectionPoint);
		window.draw(c);
	}
	

	window.setView(window.getDefaultView());
	window.draw(*m_snapshotGraph);
	window.draw(*m_predictionGraph);
	window.draw(text);



}

void PlayingScreen::updateScoreboard()
{
	auto bottomPanel = m_scoreBoard->get<tgui::Panel>("bottomPanel");
	std::size_t i = 0;

	auto grid = bottomPanel->get<tgui::Grid>("grid");
	grid->removeAllWidgets();

	for (const auto & info : m_players)
	{
		auto line = tgui::Panel::create({ tgui::bindWidth(bottomPanel) - 4, tgui::bindHeight(bottomPanel) / 10 });
		line->getRenderer()->setOpacity(.6f);

		if (i % 2)
			line->getRenderer()->setBackgroundColor(sf::Color(173, 216, 230));
		else
			line->getRenderer()->setBackgroundColor(sf::Color(176, 224, 230));

		auto name = tgui::Label::create(info.name);
		name->setSize("33%", "100%");
		name->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
		name->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
		line->add(name, "name");
		name->setInheritedOpacity(1.f);
		name->getRenderer()->setTextColor(sf::Color::Black);

		auto score = tgui::Label::create(std::to_string(info.kills) + "/" + std::to_string(info.deaths) + "/" + std::to_string(info.assists));
		score->setPosition({ tgui::bindRight(name), tgui::bindTop(name) });
		score->setSize({ "33%", "100%" });
		score->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
		score->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
		line->add(score, "mode");
		score->setInheritedOpacity(1.f);
		score->getRenderer()->setTextColor(sf::Color::Black);

		auto ping = tgui::Label::create(std::to_string(info.ping));
		ping->setPosition({ tgui::bindRight(score), tgui::bindTop(score) });
		ping->setSize({ "33%", "100%" });
		ping->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
		ping->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
		line->add(ping, "status");
		ping->setInheritedOpacity(1.f);
		ping->getRenderer()->setTextColor(sf::Color::Black);

		grid->addWidget(line, i, 0);
		++i;
	}
}

PlayingScreen::PlayerInfo * PlayingScreen::getPlayerInfo(int id)
{
	for (auto & p : m_players)
		if (p.id == id)
			return &p;
	return nullptr;

}

const PlayingScreen::PlayerInfo * PlayingScreen::getPlayerInfoByEntity(int id)
{
	for (const auto & p : m_players)
		if (p.entityId == id)
			return &p;
	return nullptr;
}

int PlayingScreen::getMyPlayerId()
{
	return m_myPlayerId;
}

Particles & PlayingScreen::getParticles()
{
	return m_particles;
}

const sf::Font * PlayingScreen::getFont()
{
	return m_font;
}

