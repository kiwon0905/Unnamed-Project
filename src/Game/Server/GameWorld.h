#pragma once

#include "Core/Packer.h"
#include "Game/GameCore.h"
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

	void onDisconnect(Peer & peer);
	void prepare(std::vector<std::unique_ptr<Peer>> & players);
	void start();
	void update(float dt, std::vector<std::unique_ptr<Peer>> & players);
	void reset();

	void onRequestInfo(Peer & peer);
private:
	Entity * createEntity(EntityType type, Peer * p);
	Entity * getEntity(int id, EntityType type);

	void sync(Peer & peer);

	int m_tick = 0;
	bool m_reset = false;
	bool m_started = false;
	int m_nextEntityId = ENTITY_ID_MIN;

	std::vector<std::vector<std::unique_ptr<Entity>>> m_entitiesByType;
};
