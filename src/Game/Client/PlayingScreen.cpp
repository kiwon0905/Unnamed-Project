#include "PlayingScreen.h"
#include "Game/GameConfig.h"

#include "Game/Client/Entity/Human.h"
#include "Game/Client/Entity/Zombie.h"
#include "Game/Client/Entity/Projectile.h"

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

	float aspectRatio = static_cast<float>(client.getWindow().getSize().x) / static_cast<float>(client.getWindow().getSize().y);
	float verticalCameraSize = static_cast<float>(client.getWindow().getSize().y);
	float horizontalCameraSize = aspectRatio * verticalCameraSize;
	m_view.setSize(std::round(horizontalCameraSize), std::round(verticalCameraSize));

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
			m_map.loadFromTmx("map/" + mapName + ".tmx");
			std::cout << "map name: " << mapName << "\n";
			m_map.loadTextures(client.getAssetManager());

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
					m_predictedClock.reset(static_cast<sf::Int64>(m_startTick + 5) * TIME_PER_TICK);
					m_prevPredictedTime = static_cast<sf::Int64>(m_startTick + 5) * TIME_PER_TICK;
					m_predictedTick = m_startTick + 5;

					std::cout << "predictedTick: " << m_predictedTick << "\n";
				}
				else if(serverTick - 2 >= m_startTick) 
				{
					m_state = IN_GAME;
					m_renderClock.reset(TIME_PER_TICK * static_cast<sf::Int64>(m_startTick));
				}
			}
			else if(m_state == IN_GAME)
			{
				sf::Time target = TIME_PER_TICK * static_cast<sf::Int64>(serverTick - 2);
				sf::Time timeLeft = TIME_PER_TICK * static_cast<sf::Int64>(serverTick) - m_renderClock.getElapsedTime();
				m_renderClock.update(target, sf::seconds(1.f));
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

					sf::Time target = input.predictedTime + input.elapsed.getElapsedTime() - sf::milliseconds(timeLeft - 50);//try to send input 50 milliseconds earlier																									 
					m_predictedClock.update(target, sf::seconds(1.f));
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
				sf::Time s = TIME_PER_TICK * static_cast<sf::Int64>(tick);
				std::string time = getStringFromTime(static_cast<int>(s.asSeconds()));
				std::string line = "[" + time + "] " + info->name + ": " + msg;
				Logger::getInstance().info("Chat", line);
				m_chatBox->addLine(line);
			}

		}

		else if (msg == Msg::SV_PLAYER_LEFT)
		{
			int id;
			int tick;
			unpacker.unpack(id);
			unpacker.unpack(tick);

			
			const PlayerInfo * info = getPlayerInfo(id);
			if (info)
			{
				std::string line = info->name + " has left the game.";

				std::cout << info->name << " left at tick: " << tick << "\n";
				m_chatBox->addLine(line, sf::Color::Red);
				PlayerJoinLeave p;
				p.tick = tick;
				p.id = id;
				p.joinOrLeave = false;
				m_playerJoinOrLeaves.push_back(p);
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
	if (m_state != IN_GAME)
		return;

	if (client.getInput().getKeyState(sf::Keyboard::Return, true))
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

	//1. adjust current and next snap
	//2. handle transit
	//3. predict

	//find snapshots
	m_prevRenderTime = m_currentRenderTime;
	m_currentRenderTime = m_renderClock.getElapsedTime();
	float renderTick = m_currentRenderTime.asSeconds() * TICKS_PER_SEC;
	m_hud->setGameTime(m_currentRenderTime);

	const auto & s = m_snapshots.find(renderTick);

	SnapInfo prevSnap;
	prevSnap.tick = m_currentSnap.tick;
	prevSnap.snapshot = m_currentSnap.snapshot;

	m_currentSnap.tick = s.first->tick;
	m_currentSnap.snapshot = s.first->snapshot.get();

	m_nextSnap.tick = s.second ? s.second->tick : -1;
	m_nextSnap.snapshot = s.second ? s.second->snapshot.get() : nullptr;

	//calculate interp
	m_renderInterTick = m_nextSnap.snapshot ? (renderTick - m_currentSnap.tick) / (m_nextSnap.tick - m_currentSnap.tick) : 0.f;

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
			else if (p.first.type == NetObject::GAME_DATA_TDM || p.second->getType() == NetObject::GAME_DATA_TDM)
			{
				continue;
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

		//remove old snapshots
		m_snapshots.removeUntil(prevSnap.tick);

		//handle player leave/join
		while (!m_playerJoinOrLeaves.empty() && m_playerJoinOrLeaves[0].tick < m_currentSnap.tick)
		{
			PlayerJoinLeave p = m_playerJoinOrLeaves.front();
			m_playerJoinOrLeaves.pop_front();

			//leave
			if (!p.joinOrLeave)
			{
				m_players.erase(std::remove_if(m_players.begin(), m_players.end(), [p](const PlayerInfo & info) {return info.id == p.id; }), m_players.end());
			}
			else
			{

			}
		}


		//update scoreboard
		updateScoreboard();
	}

	//Predict
	//add time to accumulator
	m_prevPredictedTime = m_currentPredictedTime;
	m_currentPredictedTime = m_predictedClock.getElapsedTime();
	sf::Time dt = m_currentPredictedTime - m_prevPredictedTime;
	m_accumulator += dt;

	int i = 0;
	while (m_accumulator >= TIME_PER_TICK)
	{
		++i;
		m_accumulator -= TIME_PER_TICK;
		m_predictedTick++;

		//read input
		NetInput input;
		if(!m_editBox->isFocused())
			input = client.getInput().getInput(client.getWindow(), m_view);
		
		//save the input
		m_inputs[m_currentInputIndex].tick = m_predictedTick;
		m_inputs[m_currentInputIndex].input = input;
		m_inputs[m_currentInputIndex].predictedTime = m_currentPredictedTime;
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
			p->tick(TIME_PER_TICK.asSeconds(), input, m_map);
				
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

						e->tick(TIME_PER_TICK.asSeconds(), input, m_map);

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
	m_predictedInterTick = m_accumulator / TIME_PER_TICK;
	sf::Time frameTime = m_regularClock.restart();

	m_particles.update(frameTime.asSeconds());
	m_hud->update(frameTime.asSeconds());

}

void PlayingScreen::render(Client & client)
{
	if (m_state != IN_GAME)
		return;


	//camera
	const PlayerInfo * myInfo = getPlayerInfo(m_myPlayerId);
	Entity * myEntity = getEntity(myInfo->entityId);
	if (myEntity)
	{
		sf::Vector2f center = myEntity->getCameraPosition();
		sf::FloatRect area{ center - m_view.getSize() / 2.f, m_view.getSize() };
		sf::Vector2f worldSize = m_map.getWorldSize();
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
		//center.x = std::round(center.x);
		//center.y = std::round(center.y);
		m_view.setCenter(center);
	}
	else
	{
		//std::cout << "Respawning in: " << myInfo->respawnTick  << "\n";
	}
	sf::RenderWindow & window = client.getWindow();
	window.setView(m_view);
	
	//draw background
	sf::RectangleShape background;
	background.setSize(m_map.getWorldSize());
	background.setFillColor(sf::Color(0, 191, 235));
	window.draw(background);

	m_map.drawBackground(window);

	//draw tile map
	m_map.drawTiles(window);

	//draw entities
	for (auto & v : m_entitiesByType)
	{
		for (auto & e : v)
		{
			e->render();
		}
	}
	

	//particles
	window.draw(m_particles);

	
	if(client.debugRenderEnabled())
		debugRender(client, m_view);

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

float PlayingScreen::getRenderInterTick() const
{
	return m_renderInterTick;
}

sf::Time PlayingScreen::getPrevPredictedTime() const
{
	return m_prevPredictedTime;
}

sf::Time PlayingScreen::getCurrentPredictedTime() const
{
	return m_currentPredictedTime;
}

float PlayingScreen::getPredictedInterTick() const
{
	return m_predictedInterTick;
}

const PlayingScreen::SnapInfo & PlayingScreen::getCurrentSnap() const
{
	return m_currentSnap;
}

const PlayingScreen::SnapInfo & PlayingScreen::getNextSnap() const
{
	return m_nextSnap;
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
		v.position = sf::Vector2f(static_cast<float>(i * m_map.getTileSize().x), 0.f);
		v2.position = sf::Vector2f(static_cast<float>(i * m_map.getTileSize().x), static_cast<float>(m_map.getTileSize().y * m_map.getSize().y));
		arr.append(v);
		arr.append(v2);
	}
	for (int i = 0; i < m_map.getSize().y; ++i)
	{
		sf::Vertex v, v2;
		v.color = sf::Color::Black;
		v2.color = sf::Color::Black;
		v.position = sf::Vector2f(0.f, static_cast<float>(i * m_map.getTileSize().y));
		v2.position = sf::Vector2f(static_cast<float>(m_map.getSize().x * m_map.getTileSize().x), static_cast<float>(i * m_map.getTileSize().y));
		arr.append(v);
		arr.append(v2);
	}
	window.draw(arr);

	//draw mouse world pos
	sf::Vector2f mouseWorldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window), playerView);
	sf::Text text;
	text.setFont(*client.getAssetManager().get<sf::Font>("assets/font/arial.ttf"));
	int xt = static_cast<int>(std::floor(mouseWorldPos.x / m_map.getTileSize().x));
	int yt = static_cast<int>(std::floor(mouseWorldPos.y / m_map.getTileSize().y));
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

Map & PlayingScreen::getMap()
{
	return m_map;
}

PlayingScreen::PlayerInfo * PlayingScreen::getPlayerInfo(int id)
{
	for (auto & p : m_players)
		if (p.id == id)
			return &p;
	return nullptr;

}

sf::Time PlayingScreen::getPrevRenderTime() const
{
	return m_prevRenderTime;
}

sf::Time PlayingScreen::getCurrentRenderTime() const
{
	return m_currentRenderTime;
}

const PlayingScreen::PlayerInfo * PlayingScreen::getPlayerInfoByEntity(int id)
{
	for (const auto & p : m_players)
		if (p.entityId == id)
			return &p;
	return nullptr;
}

const std::vector<PlayingScreen::PlayerInfo>& PlayingScreen::getPlayerInfos()
{
	return m_players;
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

