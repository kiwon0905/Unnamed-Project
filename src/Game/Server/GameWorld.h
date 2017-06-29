#pragma once

#include "Core/Packer.h"
#include "Game/GameConfig.h"
#include "Game/Map.h"
#include "Game/Snapshot.h"
#include "Human.h"

#include <enet/enet.h>
#include <vector>
#include <unordered_map>
#include <memory>

class GameContext;

class GameWorld
{
public:
	GameWorld(GameContext * context);

	void onDisconnect(Peer & peer);

	void reset();
	void tick();
	void snap(Packer & packer);

	const Map & getMap();
	int getCurrentTick();

	template <typename T, typename... Args>
	T * createEntity(Args &&... args);
	Entity * getEntity(int id, EntityType type);
	const std::vector<std::unique_ptr<Entity>> & getEntities(EntityType type);

private:
	GameContext * m_context;
	
	std::vector<Entity *> m_newEntities;
	std::vector<std::vector<std::unique_ptr<Entity>>> m_entitiesByType;
	int m_nextEntityId = 0;
};

template<typename T, typename ...Args>
T * GameWorld::createEntity(Args && ...args)
{
	T * entity = new T(m_nextEntityId++, std::forward<Args>(args)...);
	m_newEntities.push_back(entity);
	return entity;
}
