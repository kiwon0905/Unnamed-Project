#pragma once

#include "Core/Parser.h"
#include "Core/Packer.h"

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
private:
	void parseCommands();

	void handlePacket(Unpacker & unpacker, const ENetAddress & addr);
private:
	std::unique_ptr<std::thread> m_parsingThread;
	std::atomic<bool> m_running = false;
	std::mutex m_queueMutex;
	ENetHost * m_gameServer = nullptr;
	ENetSocket m_socket;
	ENetPeer * m_masterServer = nullptr;
	Parser m_parser;
	GameWorld m_gameWorld;
};