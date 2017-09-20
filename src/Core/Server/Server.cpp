#include "Server.h"
#include "Core/Logger.h"
#include "Core/Packer.h"
#include "Core/Protocol.h"
#include "Core/ENetUtility.h"


#include "Game/Server/GameContext/Infection.h"
#include "Game/Server/GameContext/Normal.h"
#include <iostream>
#include <string>

bool Server::initialize()
{
	Parser parser;
	if (enet_initialize() != 0)
	{
		Logger::getInstance().error("Server", "Failed to initialize enet");
		return false;
	}
	
	if (!parser.loadFromFile("game-server-config.txt"))
	{
		Logger::getInstance().error("Server", "Failed to load config.txt");
		return false;
	}

	//lan,internet,private?
	if (!parser.get("mode", m_config.mode))
	{
		Logger::getInstance().error("Server", "Failed to read mode");
		return false;
	}


	if (m_config.mode == "internet")
	{
		std::string addr;
		if (!parser.get("masterAddr", addr))
		{
			Logger::getInstance().error("Server", "Failed to read masterAddr");
			return false;
		}
		m_config.masterAddress = enutil::toENetAddress(addr);
	}

	std::string addr;
	if (!parser.get("gameServerAddr", addr))
	{
		Logger::getInstance().error("Server", "Failed to read gameServerAddr");
		return false;
	}
	m_config.address = enutil::toENetAddress(addr);



	m_server = enet_host_create(&m_config.address, 32, 1, 0, 0);
	if (!m_server)
	{
		Logger::getInstance().error("Server", "Failed to create server");
		return false;
	}

	if (enet_host_compress_with_range_coder(m_server) < 0)
	{
		Logger::getInstance().error("Server", "Failed on enet_host_compress_with_range_coder");
		return false;
	}


	if (m_config.mode == "internet")
	{
		m_masterServer = enet_host_connect(m_server, &m_config.masterAddress, 1, 1);
		if (!m_masterServer)
		{
			Logger::getInstance().error("Server", "Failed to connect to master server");
			return false;
		}
		Logger::getInstance().info("Server", "Connecting to master server...");
		//wait up to 5 seconds to connect with master server
		ENetEvent event;
		if (enet_host_service(m_server, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
		{
			enet_peer_timeout(event.peer, ENET_PEER_TIMEOUT_LIMIT, 500, 1000);
			Logger::getInstance().info("Server", "Connected to master server");
		}
		else
		{
			Logger::getInstance().error("Server", "Failed to connect to master server");
			enet_peer_reset(m_masterServer);
			return false;
		}
		//Register game
		Packer packer;
		packer.pack(Msg::SV_REGISTER_SERVER);
		packer.pack(std::string("Fun"));

		enutil::send(packer, m_masterServer, true);
	}

	Logger::getInstance().info("Server", "Succesfully initialized server at " + enutil::toString(m_server->address));
	m_running = true;
	m_parsingThread.reset(new std::thread(&Server::handleCommands, this));

	m_gameContext.reset(new Normal);
	return true;
}

void Server::run()
{
	if (initialize())
	{
		while (m_running)
		{

			handleNetwork();
			update();
			sf::sleep(sf::milliseconds(1));
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
	if (m_server)
		enet_host_destroy(m_server);
	enet_deinitialize();
}

void Server::flushPackets()
{
	enet_host_flush(m_server);
}

void Server::handleCommands()
{
	while (m_running)
	{
		std::string line;
		std::getline(std::cin, line);
		if (line == "quit")
			m_running = false;
		else if (line == "start_dev")
		{
			m_gameContext->startRound();
		
			/*if (m_players.size() > 0)
			{
				m_gameWorld.load(*this);
				m_state = LOADING;
			}
			else
			{
				Logger::getInstance().info("Need one or more player to start");
			}*/
		}
	}

}

void Server::handleNetwork()
{
	ENetEvent event;
	while (enet_host_service(m_server, &event, 0) > 0)
	{
		if (event.type == ENET_EVENT_TYPE_CONNECT)
		{
			Logger::getInstance().info("Server", enutil::toString(event.peer->address) + " connected");
			enet_peer_timeout(event.peer, ENET_PEER_TIMEOUT_LIMIT, 500, 3000);
		}
		else if (event.type == ENET_EVENT_TYPE_RECEIVE)
		{
			Unpacker unpacker;
			enutil::receive(unpacker, event.packet);
			Msg msg;
			unpacker.unpack(msg);


			if(event.peer != m_masterServer)
				m_gameContext->onMsg(msg, unpacker, event.peer);
			enet_packet_destroy(event.packet);
		}
		else if (event.type == ENET_EVENT_TYPE_DISCONNECT)
		{
			if (event.peer == m_masterServer)
			{
				Logger::getInstance().info("Server", "Disconnectd from master server");
			}
			else
			{
				
				Logger::getInstance().info("Server", enutil::toString(event.peer->address) + " disconnected");


				m_gameContext->onDisconnect(*event.peer);
			}
		}
	}
}

void Server::update()
{
	/**
	if (m_state == PRE_GAME)
	{

	}
	else if (m_state == LOADING)
	{
		//TODO: loading timeouts, etc...
	}
	else if (m_state == IN_GAME)
	{
		m_gameWorld.update(*this);
	}*/
	m_gameContext->update();
}