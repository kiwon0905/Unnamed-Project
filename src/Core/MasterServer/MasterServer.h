#pragma once

#include "Core/IdPool.h"
#include "Core/Packer.h"
#include "Core/Protocol.h"
#include "Core/Logger.h"

#include <enet/enet.h>
#include <iostream>
#include <string>
#include <map>
#include <thread>
#include <atomic>

class MasterServer
{
public:
	void run();

private:
	bool initialize();
	void finalize();

	void handlePacket(Unpacker & unpacker, ENetPeer * peer);
	void handlePacket(Unpacker & unpacker, const ENetAddress & addr);
	void parseCommands();
private:
	struct GameInfo
	{
		enum Status
		{
			PRE_GAME,
			LOADING,
			IN_GAME,
			POST_GAME,
			COUNT
		};
		
		int id;
		std::string name;
		std::string modeName;
		Status status;
		int numPlayers;
	};
	ENetHost * m_server;		//maintain connection with game servers
	ENetSocket m_socket;		//communicate with players
	std::unique_ptr<std::thread> m_parsingThread;
	std::map<ENetPeer *, GameInfo> m_games;

	std::atomic<bool> m_running = false;
	IdPool<std::int32_t> m_idPool;
};