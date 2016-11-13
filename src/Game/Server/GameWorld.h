#pragma once

#include "Core/Packer.h"
#include "Game/Server/Player.h"
#include "Entity.h"

#include <enet/enet.h>
#include <vector>
#include <unordered_map>
#include <memory>

class Server;

class GameWorld
{
public:
	void onDisconnect(ENetPeer * peer);
	void handlePacket(Unpacker & unpacker, ENetPeer * peer);
	void tick(float dt);
	void sync();
	void reset();

	Entity * createEntity(Player * player);
private:
	std::vector<Player> m_players;
	std::unordered_map<ENetPeer *, Player *> m_peerPlayers;	//map peer -> player
	bool m_reset = false;
	std::vector<std::unique_ptr<Entity>> m_entities;
	sf::Uint32 m_nextEntityId = 0;
	sf::Uint32 m_nextSnapshotSeq = 0;
};