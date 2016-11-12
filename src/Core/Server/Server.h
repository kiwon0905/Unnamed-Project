#pragma once

#include "Core/Parser.h"
#include "Core/Packet.h"

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
	bool send(const Packet & packet, ENetPeer * peer, bool reliable);
	void parseCommands();

	void handlePacket(Msg msg, Packet & packet, const ENetAddress & addr);
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