#include "GameWorld.h"
#include "Game/GameCore.h"
#include "Game/NetObject.h"

#include "Core/Protocol.h"
#include "Core/Client/Client.h"
#include "Core/MathUtility.h"
#include "Core/Logger.h"

GameWorld::GameWorld()
{
	m_entitiesByType.resize(EntityType::COUNT);

}

void GameWorld::onDisconnect()
{
	//m_ready = false;
}

void GameWorld::update(float dt, Client & client)
{
	m_tick++;

	if (m_state == ENTERING)
	{
		if (m_snapshots.empty())
			return;
		float firstTime = m_snapshots.front()->clientTick / static_cast<float>(TICK_RATE);
		if (firstTime < m_tick / static_cast<float>(TICK_RATE) - m_delay)
		{
			Logger::getInstance().info("Entered game");
			m_state = IN_GAME;
			Packer packer;
			packer.pack(Msg::CL_READY);
			client.getNetwork().send(packer, true);
		}
	}
	else if(m_state == IN_GAME)
	{
		Input input;
		input.bits = 0;
		if (client.getContext().window.hasFocus())
			input.bits = client.getInput().getBits();
		input.tick = m_tick;

		Packer packer;
		packer.pack(Msg::CL_INPUT);
		packer.pack<TICK_MIN, TICK_MAX>(input.tick);
		packer.pack(std::uint8_t(input.bits));
		client.getNetwork().send(packer, false);
	


		//TODO: Improve
		Entity * e = getEntity(m_player.id, m_player.type);
		NetEntity * ne = nullptr;
		if (m_snapshots.back()->entities.count(m_player.id))
			ne = m_snapshots.back()->entities[m_player.id].get();
		if (e && ne)
		{
			//TODO: only repredict when a new snapshot is received from the server
			m_player.m_inputs.push_back(input);
			while (!m_player.m_inputs.empty() && m_player.m_inputs.front().tick <= m_lastAckedInputTick)
				m_player.m_inputs.pop_front();

			CharacterCore * predictedCore = nullptr;
			for (auto & h : m_player.m_history)
			{
				if (h.first == m_lastAckedInputTick)
					predictedCore = h.second.get();
			}
			m_player.m_currentCore->rollback(ne, predictedCore);
			
			for(auto & i :m_player. m_inputs)
			{ 
				m_player.m_prevCore.reset(m_player.m_currentCore->clone());
				m_player.m_currentCore->update(dt, i.bits);
			}
			m_player.m_history.emplace_back(input.tick, m_player.m_currentCore->clone());
			while (!m_player.m_history.empty() && m_player.m_history.front().first < m_lastAckedInputTick)
				m_player.m_history.pop_front();
		}
	}
}

void GameWorld::render(Client & client)
{
	if (m_state != IN_GAME)
		return;
	float renderTick = m_tick + client.getFrameProgress();
	float renderTime = renderTick / static_cast<float>(TICK_RATE) - m_delay;

	Entity * playerEntity = getEntity(m_player.id, m_player.type);

	if (playerEntity)
	{
		sf::Vector2f camPos = lerp(m_player.m_prevCore->getPosition(), m_player.m_currentCore->getPosition(), client.getFrameProgress());
		client.getRenderer().setViewCenter(camPos.x, camPos.y);
	}



	sf::RenderStates states;
	states.texture = m_tileTexture;
	client.getContext().window.draw(m_tileVertices, states);
	//draw me
	if (playerEntity)
		playerEntity->renderFuture(client.getRenderer(), *m_player.m_prevCore, *m_player.m_currentCore, client.getFrameProgress());

	//find snapshots to interpolate between
	Snapshot * s0 = m_snapshots.back().get();
	Snapshot * s1 = nullptr;
	for (int i = m_snapshots.size() - 1; i >= 0; --i)
	{
		float time = m_snapshots[i]->clientTick / static_cast<float>(TICK_RATE);
		if (time <= renderTime)
		{
			s0 = m_snapshots[i].get();
			if (i + 1 < static_cast<int>(m_snapshots.size()))
				s1 = m_snapshots[i + 1].get();
			break;
		}
	}
	
	float interp = 0.f;
	if (s1)
	{
		float t0 = s0->clientTick / static_cast<float>(TICK_RATE);
		float t1 = s1->clientTick / static_cast<float>(TICK_RATE);
		interp = static_cast<float>(renderTime - t0) / static_cast<float>(t1 - t0);
	}

	//we need to make delete/create entities
	if (m_prevSnapshot != s0)
	{
		//create entities
		for (auto & p : s0->entities)
			if (!m_prevSnapshot || !m_prevSnapshot->entities.count(p.first))
				createEntity(p.first, p.second->getType());

		if(m_prevSnapshot)
			for (auto & p : m_prevSnapshot->entities)
				if (!s0->entities.count(p.first))
					getEntity(p.first, p.second->getType())->setAlive(false);

		m_prevSnapshot = s0;
		//delete old snapshots
		while (m_snapshots.front()->tick < m_prevSnapshot->tick)
			m_snapshots.pop_front();
	}


	//draw other entities
	for (auto & p : s0->entities)
	{
		Entity * e = getEntity(p.first, p.second->getType());
		NetEntity * fromEntity = s0->entities[p.first].get();
		NetEntity * toEntity = nullptr;
		if (s1 && s1->entities.count(p.first))
			toEntity = s1->entities[p.first].get();
		
		if (p.first != m_player.id)
			e->renderPast(client.getRenderer(), fromEntity, toEntity, interp);
	}

	auto isDead = [](std::unique_ptr<Entity> & e) {return !e->isAlive(); };
	for (auto & v : m_entitiesByType)
		v.erase(std::remove_if(v.begin(), v.end(), isDead), v.end());
}

void GameWorld::load()
{
}

template <class T>
std::ostream & operator<<(std::ostream & os, sf::Vector2<T> & v)
{
	return os << "(" << v.x << ", " << v.y << ")";
}

void GameWorld::onWorldInfo(Unpacker & unpacker, Client & client)
{
	unpacker.unpack<ENTITY_ID_MIN, ENTITY_ID_MAX>(m_player.id);
	unpacker.unpack(m_player.type);
	unpacker.unpack(m_mapName);
	m_player.m_currentCore.reset(createCharacterCore(m_player.type));
	m_player.m_prevCore.reset(createCharacterCore(m_player.type));
	
	m_map.loadFromFile("map/" + m_mapName + ".xml");
	m_tileTexture = client.getContext().assetManager.get<sf::Texture>("assets/" + m_mapName + ".png");
	//create vertex array
	const std::vector<std::vector<int>> & data = m_map.getData();
	int tileSize = m_map.getTileSize();
	m_tileVertices.setPrimitiveType(sf::PrimitiveType::Quads);
	int textureWidth = m_tileTexture->getSize().x / tileSize;
	int textureHeight = m_tileTexture->getSize().y / tileSize;
	for (std::size_t y = 0; y < data.size(); ++y)
	{
		for (std::size_t x = 0; x < data[0].size(); ++x)
		{
			if (!data[y][x])
				continue;
			sf::Vertex a, b, c, d;

			a.position = sf::Vector2f(x * tileSize, y * tileSize);
			b.position = sf::Vector2f((x + 1) * tileSize, y * tileSize);
			c.position = sf::Vector2f((x + 1) * tileSize, (y + 1) * tileSize);
			d.position = sf::Vector2f(x * tileSize, (y + 1) * tileSize);

			int tx = (data[y][x] - 1) % textureWidth;
			int ty = (data[y][x] - 1) / textureHeight;

			a.texCoords = sf::Vector2f(tx * tileSize, ty * tileSize);
			b.texCoords = sf::Vector2f((tx + 1) * tileSize, ty * tileSize);
			c.texCoords = sf::Vector2f((tx + 1) * tileSize, (ty + 1) * tileSize);
			d.texCoords = sf::Vector2f(tx * tileSize, (ty + 1) * tileSize);

					
			m_tileVertices.append(a);
			m_tileVertices.append(b);
			m_tileVertices.append(c);
			m_tileVertices.append(d);
		}
	}
	
	
	Packer packer;
	packer.pack(Msg::CL_LOAD_COMPLETE);
	client.getNetwork().send(packer, true);
	Logger::getInstance().info("Loading complete. Entering game...");
	m_state = ENTERING;
}

void GameWorld::onSnapshot(Unpacker & unpacker, Client & client)
{
	int tick;
	unpacker.unpack<TICK_MIN, TICK_MAX>(tick);

	//only process if the snapshot is new
	if (!m_snapshots.empty() && m_snapshots.back()->tick >= tick)
		return;
	
	Snapshot * s = new Snapshot;

	s->tick = tick;
	s->clientTick = m_tick + client.getFrameProgress();

	int ackedInputSeq;
	unpacker.unpack<TICK_MIN, TICK_MAX>(ackedInputSeq);

	m_lastAckedInputTick = ackedInputSeq;
	std::size_t count;
	unpacker.unpack<ENTITY_ID_MIN, ENTITY_ID_MAX>(count);

	for (std::size_t i = 0; i < count; ++i)
	{
		int id;
		EntityType type;
		unpacker.unpack<ENTITY_ID_MIN, ENTITY_ID_MAX>(id);
		unpacker.unpack(type);
		NetEntity * entity = nullptr;
		if (type == EntityType::HUMAN)
		{
			entity = new NetHuman();
		}
		entity->unpack(unpacker);
		s->entities[id].reset(entity);
	}
	m_snapshots.emplace_back(s);
}

Entity * GameWorld::createEntity(int id, EntityType type)
{
	Entity * e = nullptr;
	if (type == EntityType::HUMAN)
	{
		e =  new Human(id);
	}
	m_entitiesByType[type].emplace_back(e);
	return e;
}

Entity * GameWorld::getEntity(int id, EntityType type)
{
	for (auto & e : m_entitiesByType[type])
		if (e->getId() == id)
			return e.get();
	return nullptr;
}

CharacterCore * GameWorld::createCharacterCore(EntityType type)
{
	switch (type)
	{
	case HUMAN:
		return new HumanCore;
		break;
	default:
		return nullptr;
		break;
	}
}
