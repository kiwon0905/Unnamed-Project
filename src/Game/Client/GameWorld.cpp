#include "GameWorld.h"
#include "Core/Protocol.h"
#include "Game/Snapshot.h"
#include "Game/GameCore.h"
#include "Core/Client/Client.h"

void GameWorld::update(float dt, Client & client)
{
	Packer packer;
	packer.pack(Msg::CL_INPUT);
	packer.pack(client.getInput().getBits());
	client.getNetwork().send(packer, false);
	if (m_snapshots.size())
		std::cout << "seq: " << m_snapshots.back().seq << "\n";

	for (auto & e : m_entities)
		e->update(dt, *this);
}

void GameWorld::render(Client & client)
{
	for (auto & e : m_entities)
		e->render(client.getRenderer());
}

void GameWorld::handlePacket(Unpacker & unpacker)
{
	Msg msg;
	unpacker.unpack(msg);
	if (msg == Msg::SV_SNAPSHOT)
	{
		//discard if outdated,
		Snapshot snapshot;
		unpacker.unpack<SNAPSHOT_SEQ_MIN,SNAPSHOT_SEQ_MAX>(snapshot.seq);
		unsigned entityCount;
		unpacker.unpack<ENTITY_ID_MIN, ENTITY_ID_MAX + 1>(entityCount);

		if (m_snapshots.size() == 0 || snapshot.seq > m_snapshots.back().seq)
		{
			for (std::size_t i = 0; i < entityCount; ++i)
			{
				Snapshot::EntityInfo entityInfo;
				unsigned id;
				unpacker.unpack<ENTITY_ID_MIN, ENTITY_ID_MAX>(id);
				unpacker.unpack(entityInfo.type);
				unpacker.unpack<2>(0.f, 5000.f, entityInfo.x);
				unpacker.unpack<2>(0.f, 5000.f, entityInfo.y);
				snapshot.entityInfo[id] = entityInfo;
			}
			if (m_snapshots.size() > 20)
				m_snapshots.pop_front();
			m_snapshots.push_back(snapshot);
			processDelta();
		}
	}
}

void GameWorld::processDelta()
{
	if (m_snapshots.size() == 1)
	{
		const Snapshot & s1 = m_snapshots.front();
		//everything is created
		for (const auto & pair : s1.entityInfo)
		{
			std::cout << pair.first << " got created!\n";
			if (pair.second.type == EntityType::HUMAN)
			{
				createHuman(pair.first);
			}
		}
	}
	else if (m_snapshots.size() > 1)
	{
		const Snapshot & s0 = m_snapshots[m_snapshots.size() - 2];
		const Snapshot & s1 = m_snapshots.back();

		//look for entities that were created
		for (const auto & pair : s1.entityInfo)
		{
			unsigned id = pair.first;
			if (s0.entityInfo.count(id) == 0)
			{
				std::cout << id << " got created\n";
				if (pair.second.type == EntityType::HUMAN)
				{
					createHuman(pair.first);
				}
			}
		}
		//look for entities that were deleted
		for (const auto & pair : s0.entityInfo)
		{
			unsigned id = pair.first;
			if (s1.entityInfo.count(id) == 0)
			{
				Entity * e = getEntity(id);
				if (e)
					e->die();
				std::cout << id << " got destroyed\n";
			}
		}

	}
}

const std::deque<GameWorld::Snapshot> & GameWorld::getSnapshots()
{
	return m_snapshots;
}

Entity * GameWorld::getEntity(unsigned id)
{
	for (auto & e : m_entities)
	{
		if (e->getId() == id)
			return e.get();
	}
	return nullptr;
}

Human * GameWorld::createHuman(unsigned id)
{
	Human * human = new Human(id);
	m_entities.emplace_back(human);
	return human;
}
