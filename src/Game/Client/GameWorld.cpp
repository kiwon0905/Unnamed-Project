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
	m_snapshots.emplace_back();
	m_snapshots.back().tick = -1;
	std::cout << "entity size: " << m_snapshots.back().m_entities.size() << "\n";
}

void GameWorld::onDisconnect()
{
	//m_ready = false;
}

void GameWorld::update(float dt, Client & client)
{
	if (m_ready)
	{
		unsigned inputBits = 0;

		if (client.getContext().window.hasFocus())
			inputBits = client.getInput().getBits();

		Packer packer;
		packer.pack(Msg::CL_INPUT);
		packer.pack<INPUT_SEQ_MIN, INPUT_SEQ_MAX>(m_nextInputSeq);
		packer.pack(std::uint8_t(inputBits));
		client.getNetwork().send(packer, false);

		Input input;
		input.bits = inputBits;
		input.seq = m_nextInputSeq;

		m_inputs.push_back(input);
		m_nextInputSeq++;
		
		while (!m_inputs.empty() && m_inputs.front().seq <= m_lastAckedInpnutSeq)
			m_inputs.pop_front();


		Entity * e = getEntity(m_playerEntityId, m_playerEntityType);
		
		
		if (m_playerCore && e)
		{
			m_playerCore->rollback(m_snapshots.back().m_entities[m_playerEntityId].get());
			for(const auto & i : m_inputs)
				m_playerCore->update(dt, i.bits);
		}
	}
}

void GameWorld::render(Client & client)
{

	float renderTime = m_snapshots.back().tick / static_cast<float>(SERVER_TICK_RATE) + m_lastSnapshot.getElapsedTime().asSeconds() - m_delay;
	float t = 0.f;
	int fromIndex = m_snapshots.size() - 1;
	int toIndex = -1;
	for (int i = m_snapshots.size() - 1; i >= 0; --i)
	{
		float time = m_snapshots[i].tick / static_cast<float>(SERVER_TICK_RATE);
		if (time < renderTime)
		{
			fromIndex = i;
			if (i + 1 < static_cast<int>(m_snapshots.size()))
				toIndex = i + 1;

			break;
		}
	}

	if (toIndex != -1)
	{
		float t0 = m_snapshots[fromIndex].tick / static_cast<float>(SERVER_TICK_RATE);
		float t1 = m_snapshots[toIndex].tick / static_cast<float>(SERVER_TICK_RATE);
		t = (renderTime - t0) / (t1 - t0);
	}

	for (auto & v : m_entitiesByType)
		for (auto & e : v)
			e->setAlive(false);

	for (auto & p : m_snapshots[fromIndex].m_entities)
	{
		Entity * e = getEntity(p.first, p.second->getType());
		if (!e)
			e = createEntity(p.first, p.second->getType());
		
		e->setAlive(true);

		NetEntity * fromEntity = m_snapshots[fromIndex].m_entities[p.first].get();
		NetEntity * toEntity = nullptr;
		if (toIndex != -1 && m_snapshots[toIndex].m_entities.count(p.first))
			toEntity = m_snapshots[toIndex].m_entities[p.first].get();
		
		if (p.first != m_playerEntityId)
			e->renderPast(client.getRenderer(), fromEntity, toEntity, t);
	}
	Entity * playerEntity = getEntity(m_playerEntityId, m_playerEntityType);
	if (playerEntity)
		playerEntity->renderFuture(client.getRenderer(), *m_playerCore);

}

void GameWorld::load()
{
}

void GameWorld::onWorldInfo(Unpacker & unpacker, Client & client)
{
	unpacker.unpack<ENTITY_ID_MIN, ENTITY_ID_MAX>(m_playerEntityId);
	unpacker.unpack(m_playerEntityType);
	Logger::getInstance().debug("My id is" + std::to_string(m_playerEntityId));
	Logger::getInstance().debug("My type is" + m_playerEntityType);
	m_playerCore.reset(createCharacterCore(m_playerEntityType));
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
	
	//only process if the snapshot is new
	if (!m_snapshots.empty() && m_snapshots.back().tick > tick)
		return;

	m_snapshots.emplace_back();
	Snapshot & s = m_snapshots.back();
	s.tick = tick;

	unpacker.unpack<INPUT_SEQ_MIN, INPUT_SEQ_MAX>(m_lastAckedInpnutSeq);


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
		s.m_entities[id].reset(entity);

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

	//std::cout << "received snapshot for tick: " << s.tick << "\n";
	//std::cout << "0: " << m_snapshots.front().m_entities.size() << "\n";
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
