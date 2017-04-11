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

	Entity * createEntity(EntityType type, Peer * p = nullptr);
	Entity * getEntity(int id, EntityType type);
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
