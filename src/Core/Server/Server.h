#pragma once

#include "Core/Parser.h"
#include "Core/Packer.h"
#include "Core/Protocol.h"
#include "Core/IdPool.h"

#include "Game/Server/GameContext.h"
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

	const std::vector<std::unique_ptr<Peer>> & getPeers();
	Peer * getPeer(int id);
	Peer * getPeer(const ENetPeer * peer);
	//Peer * getPeerByEntityId(int id);
	bool ensurePlayers(Peer::State state);
	void broadcast(const Packer & packer, bool reliable, const Peer * exclude = nullptr);
private:
	struct Config
	{
		std::string mode;
		ENetAddress address;
		ENetAddress masterAddress;
		ENetAddress pingCheckAddr;
	};
	enum State
	{
		PRE_GAME,
		LOADING,
		IN_GAME,
		POST_GAME,
		COUNT
	};

	void handleCommands();
	void handleNetwork();
	void update();
	void reset();
	void sendServerInfoToMasterServer();

	std::unique_ptr<std::thread> m_parsingThread;
	std::atomic<bool> m_running = false;
	ENetHost * m_server = nullptr;
	ENetPeer * m_masterServer = nullptr;
	ENetSocket m_socket;
	Config m_config;

	State m_state = PRE_GAME;
	int m_nextPeerId = 3;
	std::vector<std::unique_ptr<Peer>> m_peers;
	std::unique_ptr<GameContext> m_gameContext;
};