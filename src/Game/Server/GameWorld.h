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

class Server;

class GameWorld
{
public:
	GameWorld();


	void prepare(Server & server);

	void onDisconnect(Peer & peer, Server & server);
	void onRequestGameInfo(Peer & peer, Server & server);
	void onInput(Peer & peer, Server & server, Unpacker & unpacker);

	void start();
	void reset();
	void update(Server & server);

	int getCurrentTick();
	const Map & getMap();

	template <typename T, typename... Args>
	T * createEntity(Args &&... args);
	Entity * getEntity(int id, EntityType type);
	const std::vector<std::unique_ptr<Entity>> & getEntities(EntityType type);

private:
	void snap(Server & server);

	bool m_reset = false;

	sf::Clock m_clock;
	int m_tick = 0;

	std::string m_mapName;
	Map m_map;
	
	std::vector<std::vector<std::unique_ptr<Entity>>> m_entitiesByType;
	int m_nextEntityId = 0;
	SnapshotContainer m_snapshots;
};

template<typename T, typename ...Args>
T * GameWorld::createEntity(Args && ...args)
{
	T * entity = new T(m_nextEntityId++, std::forward<Args>(args)...);
	m_entitiesByType[static_cast<int>(entity->getType())].emplace_back(entity);
	return entity;
}
