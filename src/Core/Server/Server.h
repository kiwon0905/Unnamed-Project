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
private:
	struct Config
	{
		std::string mode;
		ENetAddress address;
		ENetAddress masterAddress;
	};

	void handleCommands();
	void handleNetwork();
	void update();

	void sendServerInfoToMasterServer();

	std::unique_ptr<std::thread> m_parsingThread;
	std::atomic<bool> m_running = false;
	ENetHost * m_server = nullptr;
	ENetPeer * m_masterServer = nullptr;
	Config m_config;

	std::unique_ptr<GameContext> m_gameContext;
};