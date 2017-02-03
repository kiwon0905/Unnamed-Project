#include "GameWorld.h"

#include "Core/Protocol.h"
#include "Game/GameCore.h"
#include "Core/Client/Client.h"
#include "Core/MathUtility.h"
#include "Core/Logger.h"

GameWorld::GameWorld()
{
	m_entitiesByType.resize(EntityType::COUNT);
	m_snapshots.emplace_back();
	m_snapshots.back().tick = -1;
}

void GameWorld::onDisconnect()
{
	//m_ready = false;
}

void GameWorld::update(float dt, Client & client)
{
	if (m_ready)
	{

	}
}

void GameWorld::render(Client & client)
{

	float renderTime = m_snapshots.back().tick / static_cast<float>(SERVER_TICK_RATE) + m_lastSnapshot.getElapsedTime().asSeconds() - m_delay;

	int from = m_snapshots.size() - 1;
	int to = -1;
	for (int i = m_snapshots.size() - 1; i >= 0; --i)
	{
		float time = m_snapshots[i].tick / static_cast<float>(SERVER_TICK_RATE);
		if (time < renderTime)
		{
			from = i;
			if (i + 1 < m_snapshots.size())
				to = i + 1;

			break;
		}
	}
	for (auto & v : m_entitiesByType)
	{
		for (auto & e : v)
			e->setAlive(false);
	}
	for (auto & info : m_snapshots[from].info)
	{
		Entity * e = getEntity(info.id, info.type);
		//create if not existing.
		if (!e)
		{
			e = createEntity(info.id, info.type);
			std::cout << "created entity: " << info.id << "\n";
		}
	}

	if (m_ready)
	{
		for (auto & v : m_entitiesByType)
		{
			for (auto & e : v)
				e->render(client.getRenderer());
		}
	}
}

void GameWorld::load()
{
}

void GameWorld::onWorldInfo(Unpacker & unpacker, Client & client)
{
	unpacker.unpack<ENTITY_ID_MIN, ENTITY_ID_MAX>(m_playerEntityId);
	Logger::getInstance().debug("My id is" + std::to_string(m_playerEntityId));
	Packer packer;
	packer.pack(Msg::CL_LOAD_COMPLETE);
	client.getNetwork().send(packer, true);
	Logger::getInstance().info("Loading complete. Entering game...");
	m_loaded = true;
}

void GameWorld::onSnapshot(Unpacker & unpacker, Client & client)
{
	int tick;
	unpacker.unpack<TICK_MIN, TICK_MAX>(tick);
	
	if (!m_snapshots.empty() && m_snapshots.back().tick > tick)
		return;

	m_snapshots.emplace_back();
	Snapshot & s = m_snapshots.back();
	s.tick = tick;

	for (int type = 0; type < EntityType::COUNT; ++type)
	{
		std::size_t count;
		unpacker.unpack<ENTITY_ID_MIN, ENTITY_ID_MAX>(count);
		for (std::size_t i = 0; i < count; ++i)
		{
			int id;
			unpacker.unpack<ENTITY_ID_MIN, ENTITY_ID_MAX>(id);
			s.info.emplace_back();
			s.info.back().type = static_cast<EntityType>(type);
			s.info.back().id = id;
		}
	}

	if (!m_ready)
	{
		float renderTime = m_snapshots.back().tick / static_cast<float>(SERVER_TICK_RATE) - m_delay;
		float firstTime = m_snapshots.front().tick / static_cast<float>(SERVER_TICK_RATE);
		if (firstTime < renderTime)
		{
			Logger::getInstance().info("Entered game");
			m_ready = true;
			Packer packer;
			packer.pack(Msg::CL_READY);
			client.getNetwork().send(packer, true);
		}
	}
	





	m_lastSnapshot.restart();
}

const std::deque<GameWorld::Snapshot> & GameWorld::getSnapshots()
{
	return m_snapshots;
}

const std::deque <GameWorld::Input> & GameWorld::getInputs()
{
	return m_inputs;
}

Entity * GameWorld::createEntity(int id, EntityType type)
{
	Entity * e = nullptr;
	if (type == EntityType::HUMAN)
	{
		e =  new Human(id);
	}
	else if (type == EntityType::ZOMBIE)
	{
		e =  nullptr;
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
