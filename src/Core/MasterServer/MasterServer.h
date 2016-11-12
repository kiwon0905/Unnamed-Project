#pragma once

#include "IdPool.h"
#include "Core/Packet.h"
#include "Core/Protocol.h"
#include "Core/Logger.h"

#include <enet/enet.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <atomic>

class MasterServer
{
public:
	void run();

private:
	bool initialize();
	void finalize();

	void handlePacket(Msg msg, Packet & packet, ENetPeer * peer);
	void handlePacket(Msg msg, Packet & packet, const ENetAddress & addr);
	void parseCommands();
private:
	struct GameInfo
	{
		unsigned id;
		std::string name;
	};
	ENetHost * m_server;		//maintain connection with game servers
	ENetSocket m_socket;		//communicate with players
	std::unique_ptr<std::thread> m_parsingThread;
	std::unordered_map<ENetPeer *, GameInfo> m_games;

	std::atomic<bool> m_running = false;
	IdPool<sf::Int16> m_idPool;
};