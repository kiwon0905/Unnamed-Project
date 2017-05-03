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
	sf::Time dt = m_clock.getElapsedTime() - m_snap;

	if (dt >= m_converge)
		return m_target + dt - m_converge;
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
	packer.pack(Msg::CL_REQUEST_GAME_INFO);
	client.getNetwork().send(packer, true);

	float verticleCameraSize = 3200.f;
	float horizontalCameraSize = verticleCameraSize / client.getContext().window.getSize().x * client.getContext().window.getSize().y;
	m_view.setSize(verticleCameraSize, horizontalCameraSize);
	m_renderTexture.create(static_cast<unsigned>(verticleCameraSize + .5f), static_cast<unsigned>(horizontalCameraSize + .5f));
	m_renderTexture.setSmooth(true);

	const sf::Font * font = client.getContext().assetManager.get<sf::Font>("arial.ttf");
	m_predictionGraph = std::make_unique<Graph>(-50.f, 100.f, *font, "Prediction timing(ms)");
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
					m_predictedTime.reset(sf::seconds(static_cast<float>(m_startTick + 10) / TICKS_PER_SEC));
					m_prevPredictedTime = sf::seconds(static_cast<float>(m_startTick + 10) / TICKS_PER_SEC);
					m_predictedTick = m_startTick + 10;
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
				m_renderTime.update(target, sf::seconds(1.f));
				m_repredict = true;

				sf::Time difference = sf::seconds(serverTick / TICKS_PER_SEC) - m_renderTime.getElapsedTime();
				m_snapshotGraph->addSample(difference.asMicroseconds() / 1000.f);
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
				if (input.tick == inputTick)
				{
					sf::Time target = input.predictedTime + input.elapsed.getElapsedTime() - sf::milliseconds(timeLeft - 50);
					m_predictedTime.update(target, sf::seconds(1.f));
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

		while (m_accumulator >= sf::seconds(1.f / TICKS_PER_SEC))
		{
			m_predictedTick++;

			//inputs
			uint32_t i = 0;
			if(client.getContext().window.hasFocus())
				i = client.getInput().getBits();
			m_inputs[m_currentInputIndex].data = i;
			m_inputs[m_currentInputIndex].tick = m_predictedTick;
			m_inputs[m_currentInputIndex].predictedTime = current;
			m_inputs[m_currentInputIndex].elapsed.restart();
			m_currentInputIndex++;
			m_currentInputIndex = m_currentInputIndex % 200;

			//send to server
			Packer packer;
			packer.pack(Msg::CL_INPUT);
			packer.pack<0, MAX_TICK>(m_predictedTick);
			packer.pack(i);
			client.getNetwork().send(packer, false);

			//predict
			for (auto & v : m_entitiesByType)
			{
				for (auto & e : v)
				{
					if (e->isPredicted())
					{
						e->tick(sf::seconds(1.f / TICKS_PER_SEC).asSeconds(), i, m_map);	
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
							e->rollback(*s);
						}
					}
				}


				for (int t = m_lastSnapshotTick + 1; t <= m_predictedTick; ++t)
				{
					//TODO improve
					unsigned input = 0;
					for (const auto & i : m_inputs)
						if (i.tick == t)
							input = i.data;
					
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

				}
				m_repredict = false;
			}

			m_accumulator -= sf::seconds(1 / TICKS_PER_SEC);
		}

	}
}

void PlayingScreen::render(Client & client)
{
	if (m_state != IN_GAME)
		return;

	m_renderTexture.clear();
	sf::RectangleShape background;
	background.setSize(static_cast<sf::Vector2f>(m_map.getSize() * m_map.getTileSize()));
	m_renderTexture.draw(background);

	//tile map
	sf::RenderStates states;
	states.texture = m_tileTexture;
	m_renderTexture.draw(m_tileVertices, states);

	float renderTick = m_renderTime.getElapsedTime().asSeconds() * TICKS_PER_SEC;
	const auto & s = m_snapshots.find(renderTick);

	//calculate interpolation
	float t = 0.f;
	float predT = m_accumulator / sf::seconds(1.f / TICKS_PER_SEC);

	if (s.second)
		t = (renderTick - s.first->tick) / (s.second->tick - s.first->tick);

	//entities
	for (auto & p : s.first->snapshot->getEntities())
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
			default:
				break;
			}
			m_entitiesByType[static_cast<int>(e->getType())].emplace_back(e);
	
			if (p.first == m_myPlayer.entityId)
				e->setPrediction(true);

		}

		//check if this entity doesn't exist in the next snapshot
		if (s.second && !s.second->snapshot->getEntity(e->getId()))
			e->setAlive(true);


		Snapshot * s0 = s.first->snapshot.get();
		Snapshot * s1 = nullptr;
		if (s.second)
			s1 = s.second->snapshot.get();

		if (e->isPredicted())
			e->render(s0, s1, predT, m_renderTexture);
		else
			e->render(s0, s1, t, m_renderTexture);
	}

	//camera
	Entity * e = getEntity(m_myPlayer.entityId);
	if (e)
	{
		m_view.setCenter(e->getPosition());
		m_renderTexture.setView(m_view);
	}

	//clean up entities and delete old snapshots
	auto isDead = [](std::unique_ptr<Entity> & e) {return !e->isAlive(); };
	for (auto & v : m_entitiesByType)
		v.erase(std::remove_if(v.begin(), v.end(), isDead), v.end());
	m_snapshots.removeUntil(s.first->tick - 1);

	//draw everything to the window
	m_renderTexture.display();
	sf::Sprite sprite;
	sprite.setTexture(m_renderTexture.getTexture());
	float scaleFactor = static_cast<float>(client.getContext().window.getSize().x) / m_renderTexture.getSize().x;
	sprite.setScale(scaleFactor, scaleFactor);
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
