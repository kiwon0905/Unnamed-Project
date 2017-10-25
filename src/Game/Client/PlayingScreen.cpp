#include "PlayingScreen.h"
#include "Game/Client/Entity/Human.h"
#include "Game/Client/Entity/Zombie.h"
#include "Game/Client/Entity/Projectile.h"
#include "Game/Client/Entity/Crate.h"
#include "Game/GameConfig.h"

#include "Core/Client/Client.h"
#include "Core/Packer.h"
#include "Core/Protocol.h"
#include "Core/Logger.h"
#include "Core/Utility.h"

#include <iostream>

sf::Vector2f g_pos;

std::string getStringFromTime(int totalSeconds)
{
	int minutes = totalSeconds / 60;
	int seconds = totalSeconds % 60;

	std::string minutesString = std::to_string(minutes);
	std::string secondsString = std::to_string(seconds);
	if (minutes < 10)
		minutesString = "0" + minutesString;
	if (seconds < 10)
		secondsString = "0" + secondsString;
	return minutesString + ":" + secondsString;
}

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

void Announcer::setFont(const sf::Font & font)
{
	m_font = &font;
}

void Announcer::announce(const std::string & s, const sf::Color & fill, const sf::Color & outline)
{
	m_infos.emplace_back();
	m_infos.back().text.setFont(*m_font);
	m_infos.back().text.setFillColor(fill);
	m_infos.back().text.setOutlineColor(outline);
	m_infos.back().text.setOutlineThickness(3.f);
	m_infos.back().text.setString(s);
}

void Announcer::update(float dt)
{
	if (!m_infos.empty())
	{
		m_infos[0].time += dt;
		if (m_infos[0].time > 1.f)
		{
			float t = (m_infos[0].time - 1.f);
			int alpha = Math::lerp(255, 0, t);
			sf::Color c = m_infos[0].text.getFillColor();
			c.a = alpha;
			m_infos[0].text.setFillColor(c);
			c = m_infos[0].text.getOutlineColor();
			c.a = alpha;
			m_infos[0].text.setOutlineColor(c);
		}
		if (m_infos[0].time > 2.f)
			m_infos.pop_front();
	}
}

void Announcer::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	if (!m_infos.empty())
	{
		sf::Vector2f pos = sf::Vector2f((target.getSize().x - m_infos[0].text.getLocalBounds().width) / 2.f, target.getSize().y * 0.2f);
		states.transform.translate(pos);
		target.draw(m_infos[0].text, states);
	}
}


PlayingScreen::PlayingScreen()
{
	m_entitiesByType.resize(static_cast<std::size_t>(EntityType::COUNT));
}

void PlayingScreen::onEnter(Client & client)
{
	Packer packer;
	packer.pack(Msg::CL_REQUEST_GAME_INFO);
	client.getNetwork().send(packer, true);

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
	m_announcer.setFont(*m_font);

	//ui
	m_editBox = tgui::EditBox::create();
	m_editBox->setPosition({ 0.f, client.getWindow().getSize().y - m_editBox->getSize().y });
	m_editBox->setSize("50%", m_editBox->getSize().y);
	m_editBox->hide();


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

	m_scoreBoard = tgui::Panel::create({ "30%", "60%" });
	m_scoreBoard->getRenderer()->setBackgroundColor(sf::Color(128, 128, 128, 128));
	m_scoreBoard->setPosition({ "35%", "20%" });
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

		if (msg == Msg::SV_GAME_INFO)
		{
			std::string mapName;
			int numPlayer;
			EntityType playerEntityType;

			unpacker.unpack(mapName);
			unpacker.unpack<0, MAX_PEER_ID>(numPlayer);
			unpacker.unpack<0, MAX_PEER_ID>(m_myPlayer.id);
			unpacker.unpack(m_myPlayer.name);
			unpacker.unpack(m_myPlayer.team);
			unpacker.unpack<0, MAX_ENTITY_ID>(m_myPlayer.entityId);
			unpacker.unpack(playerEntityType);


			Logger::getInstance().info("PlayingScreen", "map is " + mapName);
			Logger::getInstance().info("PlayingScreen", m_myPlayer.name + "(" + std::to_string(m_myPlayer.id) + ") - " + "entity id: " + std::to_string(m_myPlayer.entityId) + " team: " + toString(m_myPlayer.team) + " entity type: " + std::to_string(static_cast<int>(playerEntityType)));
			for (int i = 0; i < numPlayer - 1; ++i)
			{
				PlayerInfo info;
				unpacker.unpack<0, MAX_PEER_ID>(info.id);
				unpacker.unpack(info.name);
				unpacker.unpack(info.team);
				unpacker.unpack<0, MAX_ENTITY_ID>(info.entityId);
				m_players.push_back(info);
				Logger::getInstance().info("PlayingScreen", info.name + "(" + std::to_string(info.id) + ") - " + "entity id: " + std::to_string(info.entityId) + " team: " + toString(info.team));
			}

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
			unpacker.unpack<-1, MAX_TICK>(serverTick);
			if (m_lastRecvTick >= serverTick)
				return;


			if (msg == Msg::SV_DELTA_SNAPSHOT)
			{
				int deltaTick;
				unpacker.unpack<-1, MAX_TICK>(deltaTick);
				
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
			unpacker.unpack<-1, MAX_TICK>(inputTick);
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
			uint8_t id;
			std::string msg;
			unpacker.unpack<-1, MAX_TICK>(tick);
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
			unpacker.unpack<-1, MAX_PEER_ID>(id);
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
			unpacker.unpack<-1, MAX_PEER_ID>(killedPeerId);
			unpacker.unpack<-1, MAX_PEER_ID>(killerPeerId);


			const PlayerInfo * killedPeerInfo = getPlayerInfo(killedPeerId);
			const PlayerInfo * killerPeerInfo = getPlayerInfo(killerPeerId);
			
			std::string killedPeerName = killedPeerInfo ? killedPeerInfo->name : "Unknown player";
			std::string killerPeerName = killerPeerInfo ? killerPeerInfo->name : "Unknown player";
			std::string str = killerPeerName + " killed " + killedPeerName;
			m_announcer.announce(str);
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
	if (client.getInput().isActive({ sf::Keyboard::Return }))
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
			packer.pack<-1, MAX_TICK>(m_predictedTick);
			packer.pack<-1, MAX_TICK>(m_lastRecvTick);
			input.write(packer);

			client.getNetwork().send(packer, false);
			client.getNetwork().flush();
			

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
					const void * obj = s->getEntity(e->getId());
					if (obj)
					{
						e->rollback(obj);



						for (int t = m_lastRecvTick + 1; t <= m_predictedTick; ++t)
						{
							//TODO improve
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
		m_announcer.update(timeSinceLastUpdate.asSeconds());
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
	const Snapshot * s0 = s.first->snapshot.get();
	const Snapshot * s1 = nullptr;

	m_prevRenderTick = m_currentRenderTick;
	m_currentRenderTick = s.first->tick;

	if (s.second)
		s1 = s.second->snapshot.get();

	//calculate interp
	float t = 0.f;
	float predT = m_accumulator / sf::seconds(1.f / TICKS_PER_SEC);
	if (s1)
		t = (renderTick - s.first->tick) / (s.second->tick - s.first->tick);
	
	
	//transit snapshot
	if (m_currentRenderTick != m_prevRenderTick)
	{
		//create entities
		for (auto & p : s0->getEntities())
		{
			if (p.second->getType() == NetObject::PLAYER_INFO)
			{
				continue;
			}

			Entity * e = getEntity(p.first);
			if (!e)
			{
				switch (p.second->getType())
				{
				case NetObject::HUMAN:
					e = new Human(p.first, client, *this);
					break;
				case NetObject::ZOMBIE:
					e = new Zombie(p.first, client, *this);
					break;
				case NetObject::PROJECTILE:
					e = new Projectile(p.first, client, *this);
					break;
				case NetObject::CRATE:
					e = new Crate(p.first, client, *this);
				default:
					break;
				}
				m_entitiesByType[static_cast<int>(e->getType())].emplace_back(e);



				if (p.first == m_myPlayer.entityId)
				{
					PredictedEntity * p = static_cast<PredictedEntity*>(e);
					p->setPrediction(true);
					m_predictedEntities.push_back(p);
				}
			}
		}
		//handle transient entities
		for (auto & p : s0->getTransients())
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
				if (!s0->getEntity(e->getId()))
					e->setAlive(false);
			}
		}

		auto isDead = [](std::unique_ptr<Entity> & e) {return !e->isAlive(); };
		for (auto & v : m_entitiesByType)
			v.erase(std::remove_if(v.begin(), v.end(), isDead), v.end());
		auto isDead2 = [](Entity * e) {return !e->isAlive(); };
		m_predictedEntities.erase(std::remove_if(m_predictedEntities.begin(), m_predictedEntities.end(), isDead2), m_predictedEntities.end());


		m_snapshots.removeUntil(m_prevRenderTick);
	}


	
	
	//camera
	Entity * e = getEntity(m_myPlayer.entityId);
	if (e)
	{
		sf::Vector2f center = e->getCameraPosition(s0, s1, predT, t);
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
			e->render(s0, s1, predT, t);
		}
	}


	//particles
	window.draw(m_particles);


	//Time
	window.setView(window.getDefaultView());
	sf::Text timeText;
	timeText.setFillColor(sf::Color::Blue);
	int totalSeconds = static_cast<int>(currentRenderTime.asMicroseconds()) / 1000000;
	timeText.setString(getStringFromTime(totalSeconds));
	timeText.setFont(*client.getAssetManager().get<sf::Font>("assets/font/arial.ttf"));
	timeText.setPosition(static_cast<float>(client.getWindow().getSize().x / 2.f - timeText.getLocalBounds().width / 2.f), 0.f);
	window.draw(timeText);
	
	if(client.debugRenderEnabled())
		debugRender(client, m_view);

	//announcer
	window.draw(m_announcer);
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

const PlayingScreen::PlayerInfo * PlayingScreen::getPlayerInfo(int id)
{
	if (id == m_myPlayer.id)
		return &m_myPlayer;
	for (const auto & p : m_players)
		if (p.id == id)
			return &p;
	return nullptr;

}

const PlayingScreen::PlayerInfo * PlayingScreen::getPlayerInfoByEntityId(int entityId)
{
	if (entityId == m_myPlayer.entityId)
		return &m_myPlayer;
	for (const auto & p : m_players)
		if (p.entityId == entityId)
			return &p;
	return nullptr;
}

const PlayingScreen::PlayerInfo & PlayingScreen::getMyPlayerInfo()
{
	return m_myPlayer;
}

Particles & PlayingScreen::getParticles()
{
	return m_particles;
}

const sf::Font * PlayingScreen::getFont()
{
	return m_font;
}

