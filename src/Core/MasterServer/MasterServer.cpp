#include "MasterServer.h"
#include "Core/ENetUtility.h"
#include "Core/Parser.h"

void MasterServer::run()
{
	//Initialization
	if (initialize())
	{
		while (m_running)
		{
			ENetEvent event;
			while (enet_host_service(m_server, &event, 0) > 0)
			{
				if(event.type == ENET_EVENT_TYPE_CONNECT)
				{
					std::string addr;
					enutil::toString(event.peer->address, addr);
					Logger::getInstance().info("MasterServer", addr + " connected");
					enet_peer_timeout(event.peer, ENET_PEER_TIMEOUT_LIMIT, 500, 1000);
					break;
				}
				else if(event.type == ENET_EVENT_TYPE_RECEIVE)
				{
					Unpacker unpacker(event.packet->data, event.packet->dataLength);
					handlePacket(unpacker, event.peer);
					enet_packet_destroy(event.packet);
					break;
				}
				else if(event.type == ENET_EVENT_TYPE_DISCONNECT)
				{
					std::string addr;
					enutil::toString(event.peer->address, addr);
					Logger::getInstance().info("MasterServer", addr + " removed");
					m_idPool.checkIn(m_games[event.peer].id);
					m_games.erase(event.peer);
				}
			}
			Unpacker unpacker;
			ENetAddress addr;
			while (enutil::receive(unpacker, addr, m_socket))
				handlePacket(unpacker, addr);
		}
		std::this_thread::yield();
	}
	finalize();
}

bool MasterServer::initialize()
{
	Parser parser;
	if (!parser.loadFromFile("master-server-config.txt"))
	{
		Logger::getInstance().error("MasterServer", "Failed to load master-server-config.txt");
		return false;
	}
	std::string clientAddr;
	std::string serverAddr;
	ENetAddress enetClientAddr;
	ENetAddress enetServerAddr;


	if (enet_initialize() != 0)
	{
		Logger::getInstance().error("MasterServer", "Failed to initialize enet");
		return false;
	}

	//read address
	if (!parser.get("clientAddr", clientAddr) || !enutil::toENetAddress(clientAddr, enetClientAddr))
	{
		Logger::getInstance().error("MasterServer", "Failed to read clientAddr");
		return false;
	}
	if (!parser.get("gameServerAddr", serverAddr) || !enutil::toENetAddress(serverAddr, enetServerAddr))
	{
		Logger::getInstance().error("MasterServer", "Failed to read gameServerAddr");
		return false;
	}

	//Create raw socket
	m_socket = enet_socket_create(ENET_SOCKET_TYPE_DATAGRAM);
	if (m_socket < 0)
	{
		Logger::getInstance().error("MasterServer", "Failed to create socket");
		return false;
	}
	if (enet_socket_set_option(m_socket, ENET_SOCKOPT_NONBLOCK, 1) < 0)
	{
		Logger::getInstance().error("MasterServer", "Failed to set socekt opt");
		return false;
	}


	if (enet_socket_bind(m_socket, &enetClientAddr) < 0)
	{
		Logger::getInstance().error("MasterServer", "Failed to bind socket");
		return false;
	}
	
	//Create connection
	m_server = enet_host_create(&enetServerAddr, 100, 2, 0, 0);

	if (!m_server)
	{
		Logger::getInstance().error("MasterServer", "Failed to create server");
		return false;
	}

	if (enet_host_compress_with_range_coder(m_server) < 0)
	{
		Logger::getInstance().error("MasterServer", "Failed on enet_host_compress_with_range_coder");
		return false;
	}

	m_running = true;
	std::string masterServerAddr;
	enutil::toString(m_server->address, masterServerAddr);
	Logger::getInstance().info("MasterServer", "Succesfully initialized server at " + masterServerAddr);
	m_parsingThread = std::make_unique<std::thread>(&MasterServer::parseCommands, this);


	return true;
}

void MasterServer::finalize()
{
	if(m_parsingThread)
		m_parsingThread->join();
	if(m_server)
		enet_host_destroy(m_server);
	enet_socket_destroy(m_socket);
	enet_deinitialize();
}

void MasterServer::handlePacket(Unpacker & unpacker, ENetPeer * peer)
{
	Msg msg;
	unpacker.unpack(msg);
	switch (msg)
	{

	case Msg::SV_SERVER_INFO:
	{
		std::string name;
		std::string modeName;
		GameInfo::Status status;
		int numPlayers;

		unpacker.unpack(name);
		unpacker.unpack(modeName);
		unpacker.unpack(status);
		unpacker.unpack(int32_t(numPlayers));
		
		if (m_games.count(peer) == 0)
		{
			m_games[peer].id = m_idPool.checkOut();
			std::string addr;
			enutil::toString(peer->address, addr);
			Logger::getInstance().info("MasterServer", "Server " + addr + " succesfully registerd with name: " + name + "(" + std::to_string(m_games[peer].id) + ")");
		}

		GameInfo & info = m_games[peer];
		info.name = name + " (" + std::to_string(info.id) + ")";
		info.modeName = modeName;
		info.status = status;
		info.numPlayers = numPlayers;
		break;
	}
	default:
		break;
	}
}

void MasterServer::handlePacket(Unpacker & unpacker, const ENetAddress & addr)
{
	Msg msg;
	unpacker.unpack(msg);
	if (msg == Msg::CL_REQUEST_INTERNET_SERVER_INFO)
	{
		std::string stringAddr;
		enutil::toString(addr, stringAddr);
		Logger::getInstance().info("MasterServer", "Received request for internet server list from: " + stringAddr);

		Packer packer;
		packer.pack(Msg::MSV_INTERNET_SERVER_INFO);
		packer.pack(std::uint32_t(m_games.size()));

		for (const auto & game : m_games)
		{
			packer.pack(game.first->address.host);
			packer.pack(game.first->address.port);
			packer.pack(game.second.id);
			packer.pack(game.second.name);
			packer.pack(game.second.modeName);
			packer.pack(game.second.status);
		}
		enutil::send(packer, addr, m_socket);
	}
}

void MasterServer::parseCommands()
{
	while (m_running)
	{
		std::string line;
		std::getline(std::cin, line);
		if (line == "quit")
			m_running = false;
		else if (line == "print game_list")
		{
			for (const auto & games : m_games)
			{
				std::string addr;
				enutil::toString(games.first->address, addr);
				Logger::getInstance().info("MasterServer", addr + " " + games.second.name);
			}
		}
	}
}
