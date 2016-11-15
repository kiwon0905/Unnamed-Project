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
					Logger::getInstance().info("Server " + enutil::toString(event.peer->address) + " connected");
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

					Logger::getInstance().info("Server " + enutil::toString(event.peer->address) + " removed");
					m_idPool.checkIn(m_games[event.peer].id);
					m_games.erase(event.peer);
				}
			}
			Unpacker unpacker;
			ENetAddress addr;
			while (enutil::receive(unpacker, addr, m_socket) > 0)
			{
				handlePacket(unpacker, addr);
			}
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
		Logger::getInstance().error("Failed to load master-server-config.txt");
		return false;
	}
	std::string clientAddr;
	std::string gameServerAddr;
	if (!parser.get("clientAddr", clientAddr))
	{
		Logger::getInstance().error("Failed to read clientAddr");
		return false;
	}
	if (!parser.get("gameServerAddr", gameServerAddr))
	{
		Logger::getInstance().error("Failed to read gameServerAddr");
		return false;
	}

	if (enet_initialize() != 0)
	{
		Logger::getInstance().error("Failed to initialize enet");
		return false;
	}

	//Create raw socket
	m_socket = enet_socket_create(ENET_SOCKET_TYPE_DATAGRAM);
	if (m_socket < 0)
	{
		Logger::getInstance().error("Failed to create socket");
		return false;
	}
	if (enet_socket_set_option(m_socket, ENET_SOCKOPT_NONBLOCK, 1) < 0)
	{
		Logger::getInstance().error("Failed to set socekt opt");
		return false;
	}
	if (enet_socket_bind(m_socket, &enutil::toENetAddress(clientAddr)) < 0)
	{
		Logger::getInstance().error("Failed to bind socket");
		return false;
	}
	
	//Create connection
	ENetAddress address = enutil::toENetAddress(gameServerAddr);
	m_server = enet_host_create(&address, 100, 2, 0, 0);

	if (!m_server)
	{
		Logger::getInstance().error("Failed to create server");
		return false;
	}

	if (enet_host_compress_with_range_coder(m_server) < 0)
	{
		Logger::getInstance().error("Failed on enet_host_compress_with_range_coder");
		return false;
	}

	m_running = true;
	Logger::getInstance().info("Succesfully initialized server at " + enutil::toString(m_server->address));
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
	case Msg::SV_REGISTER_SERVER:
	{
		std::string serverName;
		unpacker.unpack(serverName);

		ENetAddress addr = peer->address;

		if (m_games.count(peer))
			Logger::getInstance().error("Server already exists!");
		else
		{

			GameInfo info;
			info.id = m_idPool.checkOut();
			info.name = serverName + std::to_string(info.id);

			m_games[peer] = info;

			Logger::getInstance().info("Server " + enutil::toString(peer->address) + " succesfully registerd with name: " + serverName + "(" + std::to_string(info.id) + ")");
		}

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
	if (msg == Msg::CL_REQUEST_INTERNET_SERVER_LIST)
	{
		Logger::getInstance().info("Received request for internet server list from: " + enutil::toString(addr));

		Packer packer;
		packer.pack(Msg::MSV_INTERNET_SERVER_LIST);
		packer.pack(std::uint32_t(m_games.size()));

		for (const auto & game : m_games)
		{
			packer.pack(game.first->address.host);
			packer.pack(game.first->address.port);
			packer.pack(game.second.id);
			packer.pack(game.second.name);
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
				Logger::getInstance().info(enutil::toString(games.first->address) + " " + games.second.name);
			}
		}
	}
}
