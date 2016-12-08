#pragma once

#include "Core/Packer.h"
#include "Game/Server/Player.h"
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
	enum State
	{
		PRE,			//Entities are created and players can move around, but the game has not started
		IN_PROGRESS,	//Time clock ticks
		POST
	};

	void onDisconnect(ENetPeer * peer);
	void handlePacket(Unpacker & unpacker, ENetPeer * peer);
	void update(float dt);
	void sync();
	void reset();

	Player * getPlayer(ENetPeer * peer);
	Human * createCharacter(Player * player);
private:
	unsigned m_syncCounter = 0;
	float m_time = 0.f;
	State m_state = PRE;
	std::vector<Player> m_players;
	bool m_reset = false;
	std::vector<std::unique_ptr<Entity>> m_entities;
	int m_nextEntityId = ENTITY_ID_MIN;
	int m_nextSnapshotSeq = SNAPSHOT_SEQ_MIN;
};
