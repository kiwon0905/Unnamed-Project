#include "Server.h"
#include "Core/Logger.h"
#include "Core/Packer.h"
#include "Core/Protocol.h"
#include "Core/ENetUtility.h"

#include <iostream>
#include <string>

bool Server::initialize()
{
	srand(time(NULL));
	if (enet_initialize() != 0)
	{
		Logger::getInstance().error("Failed to initialize enet");
		return false;
	}

	if (!m_parser.loadFromFile("game-server-config.txt"))
	{
		Logger::getInstance().error("Failed to load config.txt");
		return false;
	}


	//lan or internet?
	std::string mode;
	if (!m_parser.get("mode", mode))
	{
		Logger::getInstance().error("Failed to read mode");
		return false;
	}
	std::string gameServerAddr;
	if (!m_parser.get("gameServerAddr", gameServerAddr))
	{
		Logger::getInstance().error("Failed to read gameServerAddr");
		return false;
	}

	ENetAddress addr = enutil::toENetAddress(gameServerAddr);
	m_gameServer = enet_host_create(&addr, 32, 1, 0, 0);

	if (!m_gameServer)
	{
		Logger::getInstance().error("Failed to create server");
		return false;
	}

	if (enet_host_compress_with_range_coder(m_gameServer) < 0)
	{
		Logger::getInstance().error("Failed on enet_host_compress_with_range_coder");
		return false;
	}

	if (mode == "lan")
	{
		m_socket = enet_socket_create(ENET_SOCKET_TYPE_DATAGRAM);
		if (m_socket < 0)
		{
			Logger::getInstance().error("Failed to create socket");
			return false;
		}
		if (enet_socket_set_option(m_socket, ENET_SOCKOPT_NONBLOCK, 1) < 0)
		{
			Logger::getInstance().error("Failed to set socket option");
			return false;
		}
		if(enet_socket_bind(m_socket, &enutil::toENetAddress("0.0.0.0", 12347)))
		{
			Logger::getInstance().error("Failed to bind socket");
			return false;
		}
	}
	else if (mode == "internet")
	{
		std::string masterAddr;
		m_parser.get("masterAddr", masterAddr);
		ENetAddress address = enutil::toENetAddress(masterAddr);


		m_masterServer = enet_host_connect(m_gameServer, &address, 1, 1);
		if (!m_masterServer)
		{
			Logger::getInstance().error("Failed to connect to master server");
			return false;
		}

		//wait up to 5 seconds to connect with master server
		ENetEvent event;
		if (enet_host_service(m_gameServer, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
		{
			enet_peer_timeout(event.peer, ENET_PEER_TIMEOUT_LIMIT, 500, 1000);
			Logger::getInstance().info("Connected to master server");
		}
		else
		{
			Logger::getInstance().error("Failed to connect to master server");
			enet_peer_reset(m_masterServer);
			return false;
		}
		//Register game
		Packer packer;
		packer.pack(Msg::SV_REGISTER_SERVER);
		packer.pack(std::string("Fun"));

		enutil::send(packer, m_masterServer, true);
	}

	m_running = true;
	m_parsingThread.reset(new std::thread(&Server::parseCommands, this));
	return true;
	//enet_host_compress_with_range_coder(m_gameServer);

}

void Server::run()
{
	if (initialize())
	{
		sf::Clock tickClock, syncClock;
		sf::Time elapsedTick = sf::Time::Zero, elapsedSync = sf::Time::Zero;
		const sf::Time tickInterval = sf::seconds(1 / 60.f);
		const sf::Time syncInterval = sf::seconds(1 / 20.f);

		while (m_running)
		{
			elapsedTick += tickClock.restart();
			elapsedSync += syncClock.restart();

			ENetEvent event;
			while (enet_host_service(m_gameServer, &event, 0) > 0)
			{
				if (event.type == ENET_EVENT_TYPE_CONNECT)
				{
					Logger::getInstance().info(enutil::toString(event.peer->address) + " connected");
					enet_peer_timeout(event.peer, ENET_PEER_TIMEOUT_LIMIT, 500, 1000);
					break;
				}
				else if (event.type == ENET_EVENT_TYPE_RECEIVE)
				{
					Unpacker unpacker;
					enutil::receive(unpacker, event.packet);
					m_gameWorld.handlePacket(unpacker, event.peer);
					enet_packet_destroy(event.packet);
					break;
				}
				else if (event.type == ENET_EVENT_TYPE_DISCONNECT)
				{
					if (event.peer == m_masterServer)
					{
						Logger::getInstance().info("Disconnectd from master server");
					}
					else
					{ 
						Logger::getInstance().info(enutil::toString(event.peer->address) + " disconnected");
						m_gameWorld.onDisconnect(event.peer);
					}
					break;
				}
			}

			Unpacker unpacker;
			ENetAddress addr;
			while (enutil::receive(unpacker, addr, m_socket) > 0)
			{
				handlePacket(unpacker, addr);
			}

			while (elapsedTick >= tickInterval)
			{
				m_gameWorld.update(tickInterval.asSeconds());
				elapsedTick -= tickInterval;
			}

			while (elapsedSync >= syncInterval)
			{
				m_gameWorld.sync();
				elapsedSync -= syncInterval;
			}
			std::this_thread::yield();
		}
	}


	finalize();
}

void Server::finalize()
{
	if (m_masterServer)
		enet_peer_disconnect_now(m_masterServer, 0);
	if(m_parsingThread)
		m_parsingThread->join();
	if (m_gameServer)
		enet_host_destroy(m_gameServer);
	enet_socket_destroy(m_socket);
	enet_deinitialize();
}

void Server::parseCommands()
{
	while (m_running)
	{
		std::string line;
		std::getline(std::cin, line);
		if (line == "quit")
			m_running = false;
	}

}

void Server::handlePacket(Unpacker & unpacker, const ENetAddress & addr)
{
	Msg msg;
	unpacker.unpack(msg);
	
	if (msg == Msg::CL_REVEAL_LAN_SERVER)
	{
		std::cout << "replying lan game server list\n";
		Packer packer;
		packer.pack(Msg::SV_LAN_GAME_LIST);
		packer.pack(m_gameServer->address.port);
		packer.pack("FUN");
		enutil::send(packer, addr, m_socket);
	}
}
