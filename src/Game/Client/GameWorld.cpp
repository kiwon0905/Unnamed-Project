#include "GameWorld.h"
#include "Core/Protocol.h"
#include "Game/Snapshot.h"
#include "Game/GameCore.h"
#include "Core/Client/Client.h"

void GameWorld::update(float dt, Client & client)
{
	//send input to server
	m_lastInput = client.getInput().getBits();
	Packer packer;
	packer.pack(Msg::CL_INPUT);
	packer.pack<INPUT_SEQ_MIN, INPUT_SEQ_MAX>(m_inputSeq++);
	packer.pack(m_lastInput);
	client.getNetwork().send(packer, false);

	auto isDead = [](std::unique_ptr<Entity> & e)
	{
		return e->isDead();
	};
	m_entities.erase(std::remove_if(m_entities.begin(), m_entities.end(), isDead), m_entities.end());

	for (auto & e : m_entities)
		e->update(dt, *this);
}

void GameWorld::render(Client & client)
{
	for (auto & e : m_entities)
		e->render(client.getRenderer());
}

void GameWorld::handlePacket(Unpacker & unpacker, Client & client)
{
	Msg msg;
	unpacker.unpack(msg);
	
	if (msg == Msg::SV_WORLD_INFO)
	{
		unpacker.unpack<ENTITY_ID_MIN, ENTITY_ID_MAX>(m_playerEntityId);
		std::cout << "my id is: " << m_playerEntityId << "\n";
		Packer packer;
		packer.pack(Msg::CL_READY);
		client.getNetwork().send(packer, true);

	}	
	else if (msg == Msg::SV_SNAPSHOT)
	{
		Snapshot snapshot;
		unpacker.unpack<SNAPSHOT_SEQ_MIN,SNAPSHOT_SEQ_MAX>(snapshot.seq);
		unpacker.unpack<INPUT_SEQ_MIN, INPUT_SEQ_MAX>(m_ackedInputSeq);
		unsigned entityCount;
		unpacker.unpack<ENTITY_ID_MIN, ENTITY_ID_MAX + 1>(entityCount);

		//discard if outdated,
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
				Human * h = createHuman(pair.first);
				if (pair.first == m_playerEntityId)
					h->setPrediction(true);
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
	}
}

const std::deque<GameWorld::Snapshot> & GameWorld::getSnapshots()
{
	return m_snapshots;
}

unsigned GameWorld::getInputSeq()
{
	return m_inputSeq;
}

unsigned GameWorld::getLastInput()
{
	return m_lastInput;
}

unsigned GameWorld::getAckedInputSeq()
{
	return m_ackedInputSeq;
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
