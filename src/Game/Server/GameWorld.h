#pragma once

#include "Core/Packer.h"
#include "Game/GameConfig.h"
#include "Game/Map.h"
#include "Game/Snapshot.h"

#include "Game/Server/Entity.h"

#include <enet/enet.h>
#include <vector>
#include <unordered_map>
#include <memory>

class GameContext;
class Peer;

class GameWorld
{
public:
	GameWorld(GameContext * context);

	void onDisconnect(Peer & peer);

	void reset();
	void tick();
	void snap(Snapshot & snapshot);

	template <typename T, typename... Args>
	T * createEntity(Args &&... args);
	Entity * getEntity(int id, NetObject::Type type);

	const std::vector<std::unique_ptr<Entity>> & getEntities(NetObject::Type type);
	const std::vector<Entity *> & getEntitiesOfType(const std::initializer_list<NetObject::Type> & types);

	template <typename T, typename... Args>
	T * createTransientEntity(Args &&... args);

private:
	GameContext * m_context;
	
	std::vector<std::unique_ptr<Entity>> m_newEntities;
	std::vector<std::vector<std::unique_ptr<Entity>>> m_entitiesByType;
	std::vector<std::unique_ptr<Transient>> m_transientEntities;
	int m_nextEntityId = 0;
};

template<typename T, typename ...Args>
T * GameWorld::createEntity(Args && ...args)
{
	T * entity = new T(m_nextEntityId++, m_context, std::forward<Args>(args)...);
	m_newEntities.emplace_back(entity);
	return entity;
}

template <typename T, typename... Args>
T * GameWorld::createTransientEntity(Args &&... args)
{
	T * entity = new T(std::forward<Args>(args)...);
	m_transientEntities.emplace_back(entity);
	return entity;
}