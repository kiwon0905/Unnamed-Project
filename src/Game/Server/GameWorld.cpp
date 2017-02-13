#include "GameWorld.h"
#include "Core/Server/Server.h"
#include "Core/Protocol.h"
#include "Core/ENetUtility.h"
#include "Core/Logger.h"

#include "Human.h"
#include <iostream>
#include <bitset>

GameWorld::GameWorld()
{
	m_entitiesByType.resize(EntityType::COUNT);
}

void GameWorld::onDisconnect(Peer & peer)
{
	
}

void GameWorld::prepare(std::vector<std::unique_ptr<Peer>> & players)
{
	for (auto & p : players)
	{
		Human * h = static_cast<Human*>(createEntity(EntityType::HUMAN, p.get()));
		p->setEntity(h);
		Packer packer;
		packer.pack(Msg::SV_LOAD_GAME);
		p->send(packer, true);
	}


}

void GameWorld::start()
{
	Logger::getInstance().info("The game has started.");
	m_started = true;
}

void GameWorld::update(float dt, std::vector<std::unique_ptr<Peer>> & players)
{
	for (auto & v : m_entitiesByType)
		for (auto & e : v)
			e->update(dt, *this);

	if (m_tick % 3 == 0)
	{
		for (auto & peer : players)
			sync(*peer);
	}
	m_tick++;
}

void GameWorld::sync(Peer & peer)
{
	Packer packer;
	packer.pack(Msg::SV_SNAPSHOT);
	packer.pack<TICK_MIN, TICK_MAX>(m_tick);
	packer.pack<INPUT_SEQ_MIN, INPUT_SEQ_MAX>(peer.getLastUsedInputSeq());
	std::size_t count = 0;
	for (auto & v : m_entitiesByType)
		count += v.size();

	packer.pack<ENTITY_ID_MIN, ENTITY_ID_MAX>(count);
	for (auto & v : m_entitiesByType)
	{
		for (auto & e : v)
		{
			packer.pack<ENTITY_ID_MIN, ENTITY_ID_MAX>(e->getId());
			packer.pack(e->getType());
			e->sync(packer);
		}
	}

	peer.send(packer, false);
	
}

void GameWorld::reset()
{
	m_reset = true;
}

void GameWorld::onRequestInfo(Peer & peer)
{
	Packer packer;
	packer.pack(Msg::SV_WORLD_INFO);
	packer.pack<ENTITY_ID_MIN, ENTITY_ID_MAX>(peer.getEntity()->getId());
	packer.pack(peer.getEntity()->getType());
	peer.send(packer, true);
}

Entity * GameWorld::createEntity(EntityType type, Peer * p)
{
	Entity * e;
	if (type == EntityType::HUMAN)
		e = new Human(m_nextEntityId++, p);
		
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
