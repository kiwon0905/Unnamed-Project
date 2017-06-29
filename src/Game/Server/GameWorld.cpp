#include "GameWorld.h"
#include "Core/Server/Server.h"
#include "Core/Protocol.h"
#include "Core/ENetUtility.h"
#include "Core/Logger.h"

#include "Human.h"
#include "Projectile.h"
#include <iostream>
#include <bitset>

GameWorld::GameWorld(GameContext * context):
	m_context(context)
{
	m_entitiesByType.resize(static_cast<int>(EntityType::COUNT));
}

void GameWorld::onDisconnect(Peer & peer)
{
	if (peer.getEntity())
		peer.getEntity()->setAlive(false);
}

const Map & GameWorld::getMap()
{
	return m_context->getMap();
}

int GameWorld::getCurrentTick()
{
	return m_context->getCurrentTick();
}

void GameWorld::reset()
{
	for (auto & v : m_entitiesByType)
		v.clear();
	m_newEntities.clear();
	m_nextEntityId = 0;
}

void GameWorld::tick()
{
	for (Entity * e : m_newEntities)
		m_entitiesByType[static_cast<int>(e->getType())].emplace_back(e);
	m_newEntities.clear();

	auto isDead = [](std::unique_ptr<Entity> & e) {return !e->isAlive(); };
	for (auto & v : m_entitiesByType)
		v.erase(std::remove_if(v.begin(), v.end(), isDead), v.end());

	for (auto & v : m_entitiesByType)
		for (auto & e : v)
			e->tick(sf::seconds(1.f / TICKS_PER_SEC).asSeconds(), *this);
}

void GameWorld::snap(Packer & packer)
{
	Snapshot snapshot;

	for (const auto & v : m_entitiesByType)
		for (const auto & e : v)
			e->snap(snapshot);
	snapshot.write(packer);
}

Entity * GameWorld::getEntity(int id, EntityType type)
{
	for (auto & e : m_entitiesByType[static_cast<int>(type)])
		if (e->getId() == id)
			return e.get();
	return nullptr;
}

const std::vector<std::unique_ptr<Entity>> & GameWorld::getEntities(EntityType type)
{
	return m_entitiesByType[static_cast<int>(type)];
}
