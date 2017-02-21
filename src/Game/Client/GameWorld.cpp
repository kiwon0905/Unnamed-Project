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
	std::cout << "entity size: " << m_snapshots.back().entities.size() << "\n";
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
		input.tick = m_tick;

		Packer packer;
		packer.pack(Msg::CL_INPUT);
		packer.pack<TICK_MIN, TICK_MAX>(input.tick);
		packer.pack(std::uint8_t(input.bits));
		client.getNetwork().send(packer, false);
	


		Entity * e = getEntity(m_player.id, m_player.type);
		if (e)
		{
			//TODO: only repredict when a new snapshot is received from the server
			m_inputs.push_back(input);
			while (!m_inputs.empty() && m_inputs.front().tick <= m_lastAckedInputTick)
				m_inputs.pop_front();

			CharacterCore * predictedCore = nullptr;
			for (auto & h : m_history)
			{
				if (h.first == m_lastAckedInputTick)
					predictedCore = h.second.get();
			}
			m_player.m_currentCore->rollback(m_snapshots.back().entities[m_player.id].get(), predictedCore);
			
			for(auto & i : m_inputs)
			{ 
				m_player.m_prevCore.reset(m_player.m_currentCore->clone());
				m_player.m_currentCore->update(dt, i.bits);
			}


			m_history.emplace_back(input.tick, m_player.m_currentCore->clone());
			while (!m_history.empty() && m_history.front().first < m_lastAckedInputTick)
				m_history.pop_front();
		}
	}
	m_tick++;
}

void GameWorld::render(Client & client)
{
	float renderTick = m_snapshots.back().tick + m_tick + client.getFrameProgress() - m_lastSnapshotLocalTick;
	float renderTime = renderTick / static_cast<float>(TICK_RATE) - m_delay;
		
	if (!m_snapshots.empty() && !m_ready)
	{
		float firstTime = m_snapshots.front().tick / static_cast<float>(TICK_RATE);
		if (firstTime < renderTime)
		{
			Logger::getInstance().info("Entered game");
			m_ready = true;
			Packer packer;
			packer.pack(Msg::CL_READY);
			client.getNetwork().send(packer, true);
		}
		else
			return;
	}

	//find snapshots to interpolate between
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

	//we need to make delete/create entities
	if (m_lastSnapshotTick < m_snapshots[fromIndex].tick)
	{
		//find last snapshot
		Snapshot * s = nullptr;
		for (int i = 0; i < fromIndex; ++i)
		{
			if (m_snapshots[i].tick == m_lastSnapshotTick)
			{
				s = &m_snapshots[i];
				break;
			}
		}
		//process created entities
		for (auto & p : m_snapshots[fromIndex].entities)
		{
			if (!s->entities.count(p.first))
			{
				createEntity(p.first, p.second->getType());
			}
		}

		for (auto & p : s->entities)
		{
			if (!m_snapshots[fromIndex].entities.count(p.first))
			{
				getEntity(p.first, p.second->getType())->setAlive(false);
			}
		}
		m_lastSnapshotTick = m_snapshots[fromIndex].tick;
	}



	//draw other entities

	for (auto & p : m_snapshots[fromIndex].entities)
	{
		Entity * e = getEntity(p.first, p.second->getType());
		NetEntity * fromEntity = m_snapshots[fromIndex].entities[p.first].get();
		NetEntity * toEntity = nullptr;
		if (toIndex != -1 && m_snapshots[toIndex].entities.count(p.first))
			toEntity = m_snapshots[toIndex].entities[p.first].get();
		
		if (p.first != m_player.id)
			e->renderPast(client.getRenderer(), fromEntity, toEntity, interp);
	}

	//draw me
	Entity * playerEntity = getEntity(m_player.id, m_player.type);
	if (playerEntity)
		playerEntity->renderFuture(client.getRenderer(), *m_player.m_prevCore, *m_player.m_currentCore, client.getFrameProgress());

	auto isDead = [](std::unique_ptr<Entity> & e) {return !e->isAlive(); };
	for (auto & v : m_entitiesByType)
		v.erase(std::remove_if(v.begin(), v.end(), isDead), v.end());
}

void GameWorld::load()
{
}

void GameWorld::onWorldInfo(Unpacker & unpacker, Client & client)
{
	unpacker.unpack<ENTITY_ID_MIN, ENTITY_ID_MAX>(m_player.id);
	unpacker.unpack(m_player.type);
	m_player.m_currentCore.reset(createCharacterCore(m_player.type));
	m_player.m_prevCore.reset(createCharacterCore(m_player.type));

	Packer packer;
	packer.pack(Msg::CL_LOAD_COMPLETE);
	client.getNetwork().send(packer, true);
	Logger::getInstance().info("Loading complete. Entering game...");

}

void GameWorld::onSnapshot(Unpacker & unpacker, Client & client)
{
	int tick;
	unpacker.unpack<TICK_MIN, TICK_MAX>(tick);

	//only process if the snapshot is new
	if (!m_snapshots.empty() && m_snapshots.back().tick >= tick)
		return;

	m_snapshots.emplace_back();
	Snapshot & s = m_snapshots.back();
	s.tick = tick;

	m_lastSnapshotLocalTick = m_tick + client.getFrameProgress();

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
		s.entities[id].reset(entity);
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
