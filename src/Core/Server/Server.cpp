#include "Server.h"
#include "Core/Logger.h"
#include "Core/Packer.h"
#include "Core/Protocol.h"
#include "Core/ENetUtility.h"

#include <iostream>
#include <string>

bool Server::initialize()
{
	Parser parser;
	if (enet_initialize() != 0)
	{
		Logger::getInstance().error("Failed to initialize enet");
		return false;
	}
	
	if (!parser.loadFromFile("game-server-config.txt"))
	{
		Logger::getInstance().error("Failed to load config.txt");
		return false;
	}

	//lan,internet,private?
	if (!parser.get("mode", m_config.mode))
	{
		Logger::getInstance().error("Failed to read mode");
		return false;
	}


	if (m_config.mode == "internet")
	{
		std::string addr;
		if (!parser.get("masterAddr", addr))
		{
			Logger::getInstance().error("Failed to read masterAddr");
			return false;
		}
		m_config.masterAddress = enutil::toENetAddress(addr);
	}

	std::string addr;
	if (!parser.get("gameServerAddr", addr))
	{
		Logger::getInstance().error("Failed to read gameServerAddr");
		return false;
	}
	m_config.address = enutil::toENetAddress(addr);



	m_server = enet_host_create(&m_config.address, 32, 1, 0, 0);
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


	if (m_config.mode == "internet")
	{
		m_masterServer = enet_host_connect(m_server, &m_config.masterAddress, 1, 1);
		if (!m_masterServer)
		{
			Logger::getInstance().error("Failed to connect to master server");
			return false;
		}
		Logger::getInstance().info("Connecting to master server...");
		//wait up to 5 seconds to connect with master server
		ENetEvent event;
		if (enet_host_service(m_server, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
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

	Logger::getInstance().info("Succesfully initialized server at " + enutil::toString(m_server->address));
	m_running = true;
	m_parsingThread.reset(new std::thread(&Server::handleCommands, this));
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

const std::vector<std::unique_ptr<Peer>>& Server::getPlayers() const
{
	return m_players;
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
			if (m_players.size() > 0)
			{
				m_gameWorld.load(*this);
				m_state = LOADING;
			}
			else
			{
				Logger::getInstance().info("Need one or more player to start");
			}
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
			Logger::getInstance().info(enutil::toString(event.peer->address) + " connected");
			enet_peer_timeout(event.peer, ENET_PEER_TIMEOUT_LIMIT, 500, 3000);
		}
		else if (event.type == ENET_EVENT_TYPE_RECEIVE)
		{
			Unpacker unpacker;
			enutil::receive(unpacker, event.packet);
			Msg msg;
			unpacker.unpack(msg);


			if (msg == Msg::CL_REQUEST_JOIN_GAME)
			{
				Packer packer;
				if (m_state == PRE_GAME && m_players.size() < MAX_PLAYER_ID + 1)
				{
					packer.pack(Msg::SV_ACCEPT_JOIN);
					Peer * p = new Peer(m_nextPeerId++, event.peer);
					m_players.emplace_back(p);
				}
				else
				{
					packer.pack(Msg::SV_REJECT_JOIN);
				}
				enutil::send(packer, event.peer, true);
			}

			Peer * peer = getPeer(event.peer);
			if (!peer)
				continue;


			if (msg == Msg::CL_REQUEST_ROOM_INFO)
			{

			}
			else if (msg == Msg::CL_REQUEST_GAME_INFO && m_state == LOADING)
			{
				peer->setState(Peer::LOADING);
				m_gameWorld.onRequestGameInfo(*peer, *this);
			}
			else if (msg == Msg::CL_LOAD_COMPLETE && m_state == LOADING)
			{
				peer->setState(Peer::IN_GAME);
				Logger::getInstance().info(std::to_string(peer->getId()) + " has loaded");
				if (ensurePlayers(Peer::IN_GAME))
				{
					m_state = State::IN_GAME;
					m_gameWorld.start();
					Logger::getInstance().info("Everyone has loaded");
				}
			}
			else if (msg == Msg::CL_INPUT && m_state == IN_GAME)
			{
				m_gameWorld.onInput(*peer, *this, unpacker);
			}
			enet_packet_destroy(event.packet);
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
				Peer * peer = getPeer(event.peer);
				m_gameWorld.onDisconnect(*peer, *this);
			}
		}
	}
}

void Server::update()
{
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
	}
}

Peer * Server::getPeer(const ENetPeer * peer)
{
	for (auto & p : m_players)
		if (p->getENetPeer() == peer)
			return p.get();
	return nullptr;
}

bool Server::ensurePlayers(Peer::State state)
{
	if (m_players.empty())
		return false;
	for (auto & p : m_players)
	{
		if (p->getState() != state)
			return false;
	}
	return true;
}
