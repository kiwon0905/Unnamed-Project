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
	m_entitiesByType.resize(static_cast<int>(EntityType::COUNT));
}

void GameWorld::onDisconnect(Player & player)
{
	if (player.getCharacter())
		player.getCharacter()->setAlive(false);
	auto isDead = [](std::unique_ptr<Entity> & e) {return !e->isAlive(); };
	for (auto & v : m_entitiesByType)
		v.erase(std::remove_if(v.begin(), v.end(), isDead), v.end());
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
	//add new entities
	for (auto & e : m_newEntities)
	{
		Entity * entity = e.release();
		m_entitiesByType[static_cast<int>(entity->getType())].emplace_back(entity);
	}
	m_newEntities.clear();

	//tick
	for (auto & v : m_entitiesByType)
		for (auto & e : v)
			e->tick(TIME_PER_TICK.asSeconds());

	//remove dead entities
	auto isDead = [](std::unique_ptr<Entity> & e) {return !e->isAlive(); };
	for (auto & v : m_entitiesByType)
		v.erase(std::remove_if(v.begin(), v.end(), isDead), v.end());
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

Entity * GameWorld::getEntity(int id, EntityType type)
{
	for (auto & e : m_entitiesByType[static_cast<int>(type)])
		if (e->getId() == id)
			return e.get();
	return nullptr;
}

std::vector<Entity *> GameWorld::getEntities(EntityType type)
{
	std::vector<Entity *> v;
	for (auto & e : m_entitiesByType[static_cast<int>(type)])
		v.push_back(e.get());
	return v;
}