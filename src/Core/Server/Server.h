#pragma once

#include "Core/Parser.h"
#include "Core/Packer.h"
#include "Core/Server/Peer.h"
#include "Core/Protocol.h"
#include "Core/IdPool.h"

#include "Game/Server/GameWorld.h"

#include <enet/enet.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>

class Server
{
public:
	bool initialize();
	void run();
	void finalize();

	void flushPackets();
	const std::vector<std::unique_ptr<Peer>> & getPlayers() const;
private:
	struct Config
	{
		std::string mode;
		ENetAddress address;
		ENetAddress masterAddress;
	};

	enum State
	{
		PRE_GAME,
		LOADING,
		IN_GAME
	};

	void handleCommands();
	void handleNetwork();
	void update();

	Peer * getPeer(const ENetPeer * peer);
	bool ensurePlayers(Peer::State state);

	std::unique_ptr<std::thread> m_parsingThread;
	std::atomic<bool> m_running = false;
	ENetHost * m_server = nullptr;
	ENetPeer * m_masterServer = nullptr;
	Config m_config;
	State m_state = PRE_GAME;

	int m_nextPeerId = 0;
	std::vector<std::unique_ptr<Peer>> m_players;
	GameWorld m_gameWorld;
};