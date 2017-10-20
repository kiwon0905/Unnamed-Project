#pragma once
#include "Core/Packer.h"
#include "Core/Protocol.h"

#include "Game/Server/Peer.h"

#include "Game/Server/GameWorld.h"
#include "Game/Enums.h"
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
		POST_GAME,
		COUNT
	};
	
	void onMsg(Msg msg, Unpacker & unpacker, ENetPeer * enetPeer);
	void onDisconnect(const ENetPeer & peer);
	void update();

	State getState();
	const std::vector<std::unique_ptr<Peer>> & getPeers();
	Peer * getPeer(int id);
	const Map & getMap();
	GameWorld & getWorld();
	int getCurrentTick();

	//call this in startRound to create crates
	void createCrates();
	virtual std::string getName() = 0;
	virtual void startRound() = 0;
	virtual void checkRound() = 0;
	void endRound(Team winner);
	void reset();


	//
	void announceDeath(int killedEntity, int killerEntity);
protected:
	Peer * getPeer(const ENetPeer * peer);
	Peer * getPeerByEntityId(int id);
	bool ensurePlayers(Peer::State state);
	void broadcast(const Packer & packer, bool reliable, const Peer * exclude = nullptr);


	State m_state = PRE_GAME;
	int m_nextPeerId = 0;
	std::vector<std::unique_ptr<Peer>> m_peers;

	sf::Clock m_clock;
	int m_tick = 0;
	sf::Time m_accumulator;
	sf::Time m_prevTime;
	GameWorld m_gameWorld;
	Map m_map;
	SnapshotContainer m_snapshots;
};