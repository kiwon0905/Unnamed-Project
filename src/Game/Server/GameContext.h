#pragma once
#include "Core/Packer.h"
#include "Core/Server/Peer.h"
#include "Core/Protocol.h"

#include "Core/Server/Peer.h"

#include "Game/Server/GameWorld.h"
#include <enet/enet.h>
#include <memory>


class GameContext
{
public:
	GameContext();

	enum State
	{
		PRE_GAME,
		LOADING,
		IN_GAME,
	};

	void onMsg(Msg msg, Unpacker & unpacker, ENetPeer * enetPeer);
	void onDisconnect(const ENetPeer & peer);
	void update();

	const std::vector<std::unique_ptr<Peer>> & getPeers();
	const Map & getMap();
	int getCurrentTick();

	void startMatch();
	void endMatch();
	void startRound();
	void endRound();
private:
	Peer * getPeer(const ENetPeer * peer);
	bool ensurePlayers(Peer::State state);


	State m_state = PRE_GAME;
	int m_nextPeerId = 0;
	std::vector<std::unique_ptr<Peer>> m_peers;

	sf::Clock m_clock;
	int m_tick = 0;
	GameWorld m_gameWorld;
	Map m_map;
};