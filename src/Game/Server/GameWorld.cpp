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
		Human * h = new Human(m_nextEntityId++, p.get());
		p->setEntity(h);
		Packer packer;
		packer.pack(Msg::SV_LOAD_GAME);
		p->send(packer, true);
	}


}

void GameWorld::start()
{
	Logger::getInstance().info("The game has started.");
}

void GameWorld::update(float dt, std::vector<std::unique_ptr<Peer>> & players)
{

	m_time += dt;

	m_tick++;
	if (m_tick % 3 == 0)
	{
		for (auto & peer : players)
			sync(*peer);
	}
}

void GameWorld::sync(Peer & peer)
{
	Packer packer;
	packer.pack(Msg::SV_SNAPSHOT);
	packer.pack<TICK_MIN, TICK_MAX>(m_tick);
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
	peer.send(packer, true);
}

void GameWorld::addEntity(Entity * e)
{
	int index = e->getType();
	m_entitiesByType[index].emplace_back(e);
}

