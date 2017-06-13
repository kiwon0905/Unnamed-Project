#include "PlayingScreen.h"
#include "Game/Client/Human.h"
#include "Game/Client/Projectile.h"
#include "Game/GameConfig.h"

#include "Core/Client/Client.h"
#include "Core/Packer.h"
#include "Core/Protocol.h"
#include "Core/Logger.h"
#include "Core/Utility.h"
sf::Time SmoothClock::getElapsedTime()
{
/*	sf::Time dt = m_clock.getElapsedTime() - m_snap;

	if (dt >= m_converge)
		return m_target + dt;
	else
	{
		float progress = dt / m_converge;
		return m_current + (m_target + m_converge - m_current) * progress;
	}*/

	
	sf::Time dt = m_clock.getElapsedTime() - m_snap;


	sf::Time c = m_current + dt;
	sf::Time t = m_target + dt;

	float adjustSpeed = m_adjustSpeed[0];
	if (t > c)
		adjustSpeed = m_adjustSpeed[1];

	float a = dt.asSeconds() * adjustSpeed;
	if (a > 1.0f)
		a = 1.0f;

	sf::Time r = c + (t - c)*a;
	return r;
}
void SmoothClock::reset(sf::Time target)
{
	m_clock.restart();
	m_snap = sf::Time::Zero;
	m_current = target;
	m_target = target;
	m_converge = sf::Time::Zero;

	m_adjustSpeed[0] = 0.3f;
	m_adjustSpeed[1] = 0.3f;
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

void SmoothClock::update2(sf::Time target, sf::Time timeLeft, int adjustDirection)
{
	bool needUpdate = true;
	if (timeLeft < sf::Time::Zero)
	{
		bool isSpike = false;
		if (timeLeft < sf::milliseconds(-50))
		{
			isSpike = true;

			m_spikeCounter += 5;
			if (m_spikeCounter > 50)
				m_spikeCounter = 50;
		}

		if (isSpike && m_spikeCounter < 15)
		{
			needUpdate = false;
		}
		else
		{
			if (m_adjustSpeed[adjustDirection] < 30.0f)
				m_adjustSpeed[adjustDirection] *= 2.0f;
		}
	}
	else
	{
		if (m_spikeCounter)
		{
			m_spikeCounter--;
			if (m_spikeCounter < 0)
				m_spikeCounter = 0;
		}


		m_adjustSpeed[adjustDirection] *= 0.95f;
		if (m_adjustSpeed[adjustDirection] < 2.0f)
			m_adjustSpeed[adjustDirection] = 2.0f;
	}

	if (needUpdate)
	{
		m_current = getElapsedTime();
		m_target = target;
		m_snap = m_clock.getElapsedTime();
	}
}

void SmoothClock::setAdjustSpeed(int direction, float speed)
{
	m_adjustSpeed[direction] = speed;
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
	float horizontalCameraSize = verticleCameraSize / client.getContext().window.getSize().x * client.getContext().window.getSize().y;
	m_view.setSize(verticleCameraSize, horizontalCameraSize);
	
	sf::Vector2u windowSize = client.getContext().window.getSize();
	m_renderTexture.create(windowSize.x, windowSize.y);
	m_renderTexture.setSmooth(true);

	const sf::Font * font = client.getContext().assetManager.get<sf::Font>("arial.ttf");
	m_predictionGraph = std::make_unique<Graph>(-150.f, 150.f, *font, "Prediction timing(ms)");
	m_predictionGraph->setPosition({ 0.f, 600. });
	m_snapshotGraph = std::make_unique<Graph>(-50.f, 100.f, *font, "Snapshot timing(ms)");
	m_snapshotGraph->setPosition({ 0.f, 300.f });
}

void PlayingScreen::handleEvent(const sf::Event & ev, Client & client)
{

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
			unpacker.unpack<0, MAX_PLAYER_ID>(numPlayer);
			unpacker.unpack<0, MAX_PLAYER_ID>(m_myPlayer.id);
			unpacker.unpack<0, MAX_ENTITY_ID>(m_myPlayer.entityId);
			unpacker.unpack(playerEntityType);

			std::cout << "map: " << mapName << "\n";
			std::cout << "total " << numPlayer << " players.\n";
			std::cout << "my player id: " << m_myPlayer.id << " entity id: " << m_myPlayer.entityId << " entity type: " << static_cast<int>(playerEntityType) << "\n";

			for (int i = 0; i < numPlayer - 1; ++i)
			{
				PlayerInfo info;
				unpacker.unpack<0, MAX_PLAYER_ID>(info.id);
				unpacker.unpack<0, MAX_ENTITY_ID>(info.entityId);
				m_players.push_back(info);
				std::cout << "player" << info.id << " entity: " << info.entityId << "\n";
			}

			//load map
			m_map.loadFromFile("map/" + mapName + ".xml");
			m_tileTexture = client.getContext().assetManager.get<sf::Texture>("assets/" + mapName + ".png");
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
					a.position = sf::Vector2f(x * tileSize, y * tileSize);
					b.position = sf::Vector2f((x + 1) * tileSize, y * tileSize);
					c.position = sf::Vector2f((x + 1) * tileSize, (y + 1) * tileSize);
					d.position = sf::Vector2f(x * tileSize, (y + 1) * tileSize);
					int tx = (tile - 1) % textureWidth;
					int ty = (tile - 1) / textureHeight;

					a.texCoords = sf::Vector2f(tx * tileSize, ty * tileSize) + sf::Vector2f(0.5f, 0.5f);
					b.texCoords = sf::Vector2f((tx + 1) * tileSize, ty * tileSize) + sf::Vector2f(-0.5f, 0.5f);
					c.texCoords = sf::Vector2f((tx + 1) * tileSize, (ty + 1) * tileSize) + sf::Vector2f(-0.5f, -0.5f);
					d.texCoords = sf::Vector2f(tx * tileSize, (ty + 1) * tileSize) + sf::Vector2f(0.5f, -0.5f);

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
			Logger::getInstance().info("Loading complete. Entering game...");
			m_state = ENTERING;
		}

		else if (msg == Msg::SV_SNAPSHOT)
		{
			if (m_state == LOADING)
				return;
			
			int serverTick;
			unpacker.unpack<0, MAX_TICK>(serverTick);

			if (m_lastSnapshotTick >= serverTick)
				return;

			Snapshot * snapshot = new Snapshot;
			snapshot->read(unpacker);
			m_snapshots.add(snapshot, serverTick);
			m_lastSnapshotTick = serverTick;

			if (m_state == ENTERING)
			{
				if (m_numReceivedSnapshots == 0)
				{
					m_startTick = serverTick;
					m_predictedTime.reset(sf::seconds(static_cast<float>(m_startTick + 5) / TICKS_PER_SEC));
					m_prevPredictedTime = sf::seconds(static_cast<float>(m_startTick + 5) / TICKS_PER_SEC);
					m_predictedTime.setAdjustSpeed(1, 1000.f);
					m_predictedTick = m_startTick + 5;
					std::cout << "start tick: " << m_startTick << "\n";
					std::cout << "predicted tick: " << m_predictedTick << "\n";
				}
				else if(serverTick - 2 >= m_startTick) 
				{
					m_state = IN_GAME;
					m_renderTime.reset(sf::seconds(m_startTick / TICKS_PER_SEC));
					std::cout << "startTime: " << m_startTick / TICKS_PER_SEC << " recv server time: " << serverTick / TICKS_PER_SEC << "\n";
				}
		
			}
			else if(m_state == IN_GAME)
			{
				sf::Time target = sf::seconds((serverTick - 2) / TICKS_PER_SEC);
				sf::Time timeLeft = sf::seconds(serverTick / TICKS_PER_SEC) - m_renderTime.getElapsedTime();
				m_renderTime.update2(target, timeLeft, 0);
				//m_renderTime.update(target, sf::seconds(1.f));
				m_repredict = true;

				m_snapshotGraph->addSample(timeLeft.asMicroseconds() / 1000.f);
			}
			m_numReceivedSnapshots++;
		}

		else if (msg == Msg::SV_INPUT_TIMING)
		{
			int inputTick;
			int32_t timeLeft;
			unpacker.unpack<0, MAX_TICK>(inputTick);
			unpacker.unpack(timeLeft);
			for (const auto & input : m_inputs)
			{
				if (input.input.tick == inputTick)
				{
					sf::Time target = input.predictedTime + input.elapsed.getElapsedTime() - sf::milliseconds(timeLeft - 50);
					//m_predictedTime.update(target, sf::seconds(1.f));
					m_predictedTime.update2(target, sf::milliseconds(timeLeft), 1);
					m_predictionGraph->addSample(timeLeft);
				}
			}
		}
	}

	else if (netEv.type == ENET_EVENT_TYPE_CONNECT)
	{

	}

	else if (netEv.type == ENET_EVENT_TYPE_DISCONNECT)
	{
	}
}

void PlayingScreen::handlePacket(Unpacker & unpacker, const ENetAddress & addr, Client & client)
{

}

void PlayingScreen::update(Client & client)
{
	if (m_state == IN_GAME)
	{
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
			NetInput input = client.getInput().getInput(m_renderTexture, client.getContext().window);
			input.tick = m_predictedTick;
			m_inputs[m_currentInputIndex].input = input;
			m_inputs[m_currentInputIndex].predictedTime = current;
			m_inputs[m_currentInputIndex].elapsed.restart();
			m_currentInputIndex++;
			m_currentInputIndex = m_currentInputIndex % 200;

			//send to server
			Packer packer;
			packer.pack(Msg::CL_INPUT);
			input.write(packer);
			client.getNetwork().send(packer, false);
			client.getNetwork().flush();
			
			
			//predict
			for (auto & v : m_entitiesByType)
			{
				for (auto & e : v)
				{
					if (e->isPredicted())
					{
						e->tick(sf::seconds(1.f / TICKS_PER_SEC).asSeconds(), input, m_map);	
					}
				}
			}

			//repredict
			if (m_repredict)
			{

				Snapshot * s = m_snapshots.getLast();
				for (auto & v : m_entitiesByType)
				{
					for (auto & e : v)
					{
						if (e->isPredicted())
						{
							const NetObject * obj = s->getEntity(e->getId());
							if (obj)
							{
								e->rollback(*obj);
							
							
							
								for (int t = m_lastSnapshotTick + 1; t <= m_predictedTick; ++t)
								{
									//TODO improve
									NetInput input;
									for (const auto & i : m_inputs)
										if (i.input.tick == t)
											input = i.input;

									e->tick(sf::seconds(1.f / TICKS_PER_SEC).asSeconds(), input, m_map);
								}		
							}
						}
					}
				}
				m_repredict = false;
			}
		}
		if (i > 1)
			std::cout << "double update!";
	}
}

void PlayingScreen::render(Client & client)
{
	if (m_state != IN_GAME)
		return;

	//find snapshots
	float renderTick = m_renderTime.getElapsedTime().asSeconds() * TICKS_PER_SEC;
	const auto & s = m_snapshots.find(renderTick);
	Snapshot * s0 = s.first->snapshot.get();
	Snapshot * s1 = nullptr;
	if (s.second)
		s1 = s.second->snapshot.get();

	//calculate interp
	float t = 0.f;
	float predT = m_accumulator / sf::seconds(1.f / TICKS_PER_SEC);
	if (s1)
		t = (renderTick - s.first->tick) / (s.second->tick - s.first->tick);

	//create entities
	for (auto & p : s0->getEntities())
	{
		Entity * e = getEntity(p.first);

		if (!e)
		{
			switch (p.second->getType())
			{
			case NetObject::HUMAN:
				e = new Human(p.first);
				break;
			case NetObject::PROJECTILE:
				e = new Projectile(p.first);
				break;
			default:
				break;
			}
			m_entitiesByType[static_cast<int>(e->getType())].emplace_back(e);
			if (p.first == m_myPlayer.entityId)
				e->setPrediction(true);
		}
	}

	//delete entities
	for (auto & v : m_entitiesByType)
	{
		for (auto & e : v)
		{
			if (s1 && !s1->getEntity(e->getId()))
				e->setAlive(false);
			if (!s0->getEntity(e->getId()))
				e->setAlive(false);
		}
	}

	auto isDead = [](std::unique_ptr<Entity> & e) {return !e->isAlive(); };
	for (auto & v : m_entitiesByType)
		v.erase(std::remove_if(v.begin(), v.end(), isDead), v.end());
	m_snapshots.removeUntil(s.first->tick - 1);





	//entity pre render
	for (auto & v : m_entitiesByType)
	{
		for (auto & e : v)
		{
			if (e->isPredicted())
				e->preRender(s0, s1, predT);
			else
				e->preRender(s0, s1, t);
		}
	}

	//camera
	Entity * e = getEntity(m_myPlayer.entityId);
	if (e)
	{
		m_view.setCenter(e->getPosition());
		m_renderTexture.setView(m_view);
	}

	//clear texture
	m_renderTexture.clear();
	sf::RectangleShape background;
	background.setSize(static_cast<sf::Vector2f>(m_map.getSize() * m_map.getTileSize()));
	m_renderTexture.draw(background);

	//draw tile map
	sf::RenderStates states;
	states.texture = m_tileTexture;
	m_renderTexture.draw(m_tileVertices, states);

	sf::VertexArray arr;
	arr.setPrimitiveType(sf::Lines);
	for (int i = 0; i < m_map.getSize().x; ++i)
	{
		sf::Vertex v, v2;
		v.color = sf::Color::Black;
		v2.color = sf::Color::Black;
		v.position = sf::Vector2f(i * m_map.getTileSize(), 0.f);
		v2.position = sf::Vector2f(i * m_map.getTileSize(), m_map.getTileSize() * m_map.getSize().y);
		arr.append(v);
		arr.append(v2);
	}
	for (int i = 0; i < m_map.getSize().y; ++i)
	{
		sf::Vertex v, v2;
		v.color = sf::Color::Black;
		v2.color = sf::Color::Black;
		v.position = sf::Vector2f(0.f, i * m_map.getTileSize());
		v2.position = sf::Vector2f(m_map.getSize().x * m_map.getTileSize(), i * m_map.getTileSize());
		arr.append(v);
		arr.append(v2);
	}
	m_renderTexture.draw(arr);


	//draw entities
	for (auto & v : m_entitiesByType)
	{
		for (auto & e : v)
		{
			e->render(m_renderTexture, client);
		}
	}


	//draw everything to the window
	m_renderTexture.display();
	sf::Sprite sprite;
	sprite.setTexture(m_renderTexture.getTexture());
	client.getContext().window.draw(sprite);


	client.getContext().window.draw(*m_snapshotGraph);
	client.getContext().window.draw(*m_predictionGraph);
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
