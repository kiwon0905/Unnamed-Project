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
		Input input;
		input.bits = 0;
		if (client.getContext().window.hasFocus())
			input.bits = client.getInput().getBits();
		input.seq = m_nextInputSeq;

		Packer packer;
		packer.pack(Msg::CL_INPUT);
		packer.pack<INPUT_SEQ_MIN, INPUT_SEQ_MAX>(input.seq);
		packer.pack(std::uint8_t(input.bits));
		client.getNetwork().send(packer, false);
	


		Entity * e = getEntity(m_playerEntityId, m_playerEntityType);
		if (m_playerCore && e)
		{
			m_inputs.push_back(input);
			while (m_inputs.front().seq <= m_lastAckedInputSeq)
				m_inputs.pop_front();

			CharacterCore * predictedCore = nullptr;
			for (auto & h : m_history)
			{
				if (h.first == m_lastAckedInputSeq)
					predictedCore = h.second.get();
			}


		/*	m_playerCore->rollback(m_snapshots.back().m_entities[m_playerEntityId].get(), predictedCore);
			
			for(auto & i : m_inputs)
			{ 
				m_oldPos = m_playerCore->getPosition();
				m_playerCore->update(dt, i.bits);
			}*/

			m_oldPos = m_playerCore->getPosition();
			m_playerCore->update(dt, input.bits);
			m_history.emplace_back(input.seq, m_playerCore->clone());
			while (!m_history.empty() && m_history.front().first < m_lastAckedInputSeq)
				m_history.pop_front();
		}
		m_nextInputSeq++;
		m_tickSinceLastSnapshot++;

	}
}

void GameWorld::render(float t, Client & client)
{
	float renderTime = (m_snapshots.back().tick + m_tickSinceLastSnapshot + t - 1) / static_cast<float>(TICK_RATE) - m_delay;
	
	int fromIndex = m_snapshots.size() - 1;
	int toIndex = -1;
	for (int i = m_snapshots.size() - 1; i >= 0; --i)
	{
		float time = m_snapshots[i].tick / static_cast<float>(TICK_RATE);
		if (time < renderTime)
		{
			fromIndex = i;
			if (i + 1 < static_cast<int>(m_snapshots.size()))
				toIndex = i + 1;
			break;
		}
	}

	float interp = 0.f;
	if (toIndex != -1)
	{
		float t0 = m_snapshots[fromIndex].tick / static_cast<float>(TICK_RATE);
		float t1 = m_snapshots[toIndex].tick / static_cast<float>(TICK_RATE);
		interp = static_cast<float>(renderTime - t0) / static_cast<float>(t1 - t0);
	}

	//draw other entities
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
			e->renderPast(client.getRenderer(), fromEntity, toEntity, interp);
	}

	//draw me
	Entity * playerEntity = getEntity(m_playerEntityId, m_playerEntityType);
	if (playerEntity)
	{
		sf::Vector2f pos = lerp(m_oldPos, m_playerCore->getPosition(), t);
		sf::RectangleShape r;
		r.setSize({ 100.f, 100.f });
		r.setFillColor(sf::Color::Yellow);
		r.setPosition(pos);
		client.getContext().window.draw(r);
	}
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

	m_tickSinceLastSnapshot = 0;

	m_snapshots.emplace_back();
	Snapshot & s = m_snapshots.back();
	s.tick = tick;

	unpacker.unpack<INPUT_SEQ_MIN, INPUT_SEQ_MAX>(m_lastAckedInputSeq);


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
		float renderTime = m_snapshots.back().tick / static_cast<float>(TICK_RATE) - m_delay;
		float firstTime = m_snapshots.front().tick / static_cast<float>(TICK_RATE);
		if (firstTime < renderTime)
		{
			Logger::getInstance().info("Entered game");
			m_ready = true;
			Packer packer;
			packer.pack(Msg::CL_READY);
			client.getNetwork().send(packer, true);
		}
	}
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
