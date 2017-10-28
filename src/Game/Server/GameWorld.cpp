#include "GameWorld.h"
#include "Core/Server/Server.h"
#include "Core/Protocol.h"
#include "Core/ENetUtility.h"
#include "Core/Logger.h"

#include "Entity/Human.h"
#include "Entity/Zombie.h"
#include "Entity/Projectile.h"


GameWorld::GameWorld(GameContext * context):
	m_context(context)
{
	m_entitiesByType.resize(static_cast<int>(NetObject::ENTITY_COUNT));
}

void GameWorld::onDisconnect(Peer & peer)
{
	if (peer.getEntity())
		peer.getEntity()->setAlive(false);
}

void GameWorld::reset()
{
	for (auto & v : m_entitiesByType)
		v.clear();
	m_newEntities.clear();
	m_transientEntities.clear();
	m_nextEntityId = 0;
}

void GameWorld::tick()
{
	for (auto & e : m_newEntities)
	{
		Entity * entity = e.release();
		m_entitiesByType[static_cast<int>(entity->getType())].emplace_back(entity);
	}
	m_newEntities.clear();

	auto isDead = [](std::unique_ptr<Entity> & e) {return !e->isAlive(); };
	for (auto & v : m_entitiesByType)
		v.erase(std::remove_if(v.begin(), v.end(), isDead), v.end());
	
	for (auto & v : m_entitiesByType)
		for (auto & e : v)
			e->tick(sf::seconds(1.f / TICKS_PER_SEC).asSeconds());
}

void GameWorld::snap(Snapshot & snapshot)
{
	for (const auto & v : m_entitiesByType)
		for (const auto & e : v)
			e->snap(snapshot);

	for (const auto & e : m_transientEntities)
		e->snap(snapshot);
	m_transientEntities.clear();
}

Entity * GameWorld::getEntity(int id, NetObject::Type type)
{
	for (auto & e : m_entitiesByType[static_cast<int>(type)])
		if (e->getId() == id)
			return e.get();
	return nullptr;
}

const std::vector<std::unique_ptr<Entity>> & GameWorld::getEntities(NetObject::Type type)
{
	return m_entitiesByType[static_cast<int>(type)];
}

const std::vector<Entity *> & GameWorld::getEntitiesOfType(const std::initializer_list<NetObject::Type> & types)
{
	static std::vector<Entity*> entities;

	entities.clear();

	for (NetObject::Type type : types)
	{
		for (const auto & e : m_entitiesByType[static_cast<int>(type)])
		{
			entities.push_back(e.get());
		}
	}


	return entities;
}
