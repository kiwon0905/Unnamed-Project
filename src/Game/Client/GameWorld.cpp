#include "GameWorld.h"
#include "Core/Protocol.h"
#include "Game/GameCore.h"
#include "Core/Client/Client.h"
#include "Core/MathUtility.h"

void GameWorld::update(float dt, Client & client)
{
	if (m_ready)
	{
		unsigned inputBits = 0;
		if (client.getContext().window.hasFocus())
		{
			inputBits = client.getInput().getBits();

			Input input;
			input.bits = inputBits;
			input.seq = m_inputSeq;
			m_inputs.push_back(input);

			//send input to server
			Packer packer;
			packer.pack(Msg::CL_INPUT);
			packer.pack<INPUT_SEQ_MIN, INPUT_SEQ_MAX>(m_inputSeq);
			packer.pack(inputBits);
			client.getNetwork().send(packer, false);
			m_inputSeq++;

		}

		//remove dead entities
		auto isDead = [](std::unique_ptr<Entity> & e)
		{
			return e->isDead();
		};
		m_entities.erase(std::remove_if(m_entities.begin(), m_entities.end(), isDead), m_entities.end());

		//Update entities
		for (auto & e : m_entities)
		{
			if (e->getId() == m_playerEntityId)
				e->setInput(inputBits);
			e->update(dt, *this);
		}
		Entity * e = getEntity(m_playerEntityId);
		//std::cout << m_inputSeq - 1 << ": " << e->getPosition().x << "\n";
		//interpolate();

		m_time += dt;
	}
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
		std::cout << "inputseq: " << m_inputSeq << "\n";
		Packer packer;
		packer.pack(Msg::CL_READY);
		client.getNetwork().send(packer, true);
	}	
	else if (msg == Msg::SV_SNAPSHOT)
	{
		m_ready = true;
		Snapshot snapshot;
		snapshot.time = m_time;
		unpacker.unpack<SNAPSHOT_SEQ_MIN,SNAPSHOT_SEQ_MAX>(snapshot.seq);
		unpacker.unpack<INPUT_SEQ_MIN, INPUT_SEQ_MAX>(snapshot.inputSeq);
		unsigned entityCount;
		unpacker.unpack<ENTITY_ID_MIN, ENTITY_ID_MAX + 1>(entityCount);
		
		if (m_snapshots.size() == 0)
			m_time = 0.f;
		//Don't process outdated snapshots,
		if (m_snapshots.size() == 0 || snapshot.seq > m_snapshots.back().seq)
		{
			for (std::size_t i = 0; i < entityCount; ++i)
			{
				Snapshot::EntityInfo entityInfo;
				unsigned id;
				unpacker.unpack<ENTITY_ID_MIN, ENTITY_ID_MAX>(id);
				unpacker.unpack(entityInfo.type);
				unpacker.unpack<2>(0.f, 5000.f, entityInfo.pos.x);
				unpacker.unpack<2>(0.f, 5000.f, entityInfo.pos.y);
				snapshot.entityInfo[id] = entityInfo;
			}
			if (m_snapshots.size() > 20)
				m_snapshots.pop_front();
			m_snapshots.push_back(snapshot);
			processDelta();
			predict();
		}
	}
	
}

void GameWorld::interpolate()
{
	int s0 = 0;
	int s1 = -1;
	const float delayedTime = m_time - .1f;
	for (int i = m_snapshots.size() - 1; i >= 0; --i)
	{
		if (delayedTime > m_snapshots[i].time)
		{
			s0 = i;
			if (i != m_snapshots.size() - 1)
				s1 = i + 1;
			break;
		}
	}

	//create necessary entities
//	processEntities(m_prevS0, s0);
	/*if (s1 == -1)
	{
		for (const auto & pair : m_snapshots[s0].entityInfo)
		{
			if (pair.first != m_playerEntityId)
			{
				Entity * e = getEntity(pair.first);
				e->setPosition(m_snapshots[s0].entityInfo[pair.first].pos);
			}
		}
	}
	else
	{
		float t = delayedTime / (m_snapshots[s1].time - m_snapshots[s0].time);
		for (const auto & pair : m_snapshots[s0].entityInfo)
		{
			if (pair.first != m_playerEntityId)
			{
				sf::Vector2f pos0 = m_snapshots[s0].entityInfo[pair.first].pos;
				sf::Vector2f pos1 = m_snapshots[s1].entityInfo[pair.first].pos;
				sf::Vector2f lerpedPos = lerp(pos0, pos1, t);
				Entity * e = getEntity(pair.first);
				e->setPosition(lerpedPos);
			}
		}
	}*/
	m_prevS0 = s0;
}

void GameWorld::processEntities(int prev, int current)
{
	if (prev == current)
		return;

	if (prev == -1)
	{
		const Snapshot & s = m_snapshots[current];
		for (const auto & pair : s.entityInfo)
		{
			unsigned id = pair.first;
			if (pair.second.type == EntityType::HUMAN)
			{
				Human * h = createHuman(pair.first);
				if (pair.first == m_playerEntityId)
					h->setPrediction(true);

				std::cout << "created: " << id << "\n";
			}
		}
	}
	else
	{
		const Snapshot & s0 = m_snapshots[prev];
		const Snapshot & s1 = m_snapshots[current];

		//look for entities that were created
		for (const auto & pair : s1.entityInfo)
		{
			unsigned id = pair.first;
			if (s0.entityInfo.count(id) == 0)
			{
				if (pair.second.type == EntityType::HUMAN)
				{
					Human * h = createHuman(pair.first);
					if (pair.first == m_playerEntityId)
						h->setPrediction(true);
					std::cout << "created: " << id << "\n";
				}
			}
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
		m_ready = true;
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

void GameWorld::predict()
{
	//Client side prediction
	//discard inputs that have been acked by server.
	while (m_inputs.size() && m_inputs.front().seq <= m_snapshots.back().inputSeq)
		m_inputs.pop_front();

	Entity * playerEntity = getEntity(m_playerEntityId);
	if (playerEntity)
	{
		//std::cout << "Before rollback: " << playerEntity->getPosition().x << "\n";
		float rolledbackPos = playerEntity->getPosition().x;
		
		//reapply inputs that haven't been acked
		playerEntity->setPosition(m_snapshots.back().entityInfo[m_playerEntityId].pos);
		
		//std::cout << "rolled back to position: " << playerEntity->getPosition().x << " at input seq: " << m_snapshots.back().inputSeq << "\n";
		for (auto & p : m_inputs)
		{
			playerEntity->setInput(p.bits);
			playerEntity->update(Client::TIME_STEP.asSeconds(), *this);
		}
		//std::cout << "reapplied position: " << playerEntity->getPosition().x << "\n";
		float reapplyPos = playerEntity->getPosition().x;
		if (comp(rolledbackPos, reapplyPos, 1.f) != 0)
		{
			std::cout << "Prediction error!!\n";
			std::cout << "rolledbackPos: " << rolledbackPos << " reapplyPos: " << reapplyPos << "\n\n";
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
