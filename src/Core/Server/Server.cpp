#include "Server.h"
#include "Core/Logger.h"
#include "Core/Packer.h"
#include "Core/Protocol.h"
#include "Core/ENetUtility.h"


#include "Game/Server/GameContext/Infection.h"
#include "Game/Server/GameContext/Vanilla.h"
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
		if (!parser.get("masterAddr", addr) || !enutil::toENetAddress(addr, m_config.masterAddress)) 
		{
			Logger::getInstance().error("Server", "Failed to read masterAddr");
			return false;
		}
	}

	//init network
	std::string addr;

	//game server
	if (!parser.get("gameServerAddr", addr) || !enutil::toENetAddress(addr, m_config.address))
	{
		Logger::getInstance().error("Server", "Failed to read gameServerAddr");
		return false;
	}
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

	//ping check socket
	if (!parser.get("pingCheckAddr", addr) || !enutil::toENetAddress(addr, m_config.pingCheckAddr))
	{
		Logger::getInstance().error("Server", "Failed to read pingCheckAddr");
		return false;
	}
	m_socket = enet_socket_create(ENET_SOCKET_TYPE_DATAGRAM);
	if (m_socket < 0)
	{
		Logger::getInstance().error("Server", "Failed to create a socket");
		return false;
	}
	if (enet_socket_set_option(m_socket, ENET_SOCKOPT_NONBLOCK, 1) < 0)
	{
		Logger::getInstance().error("Server", "Failed to set socket option");
		return false;
	}
	if (enet_socket_bind(m_socket, &m_config.pingCheckAddr) < 0)
	{
		Logger::getInstance().error("MasterServer", "Failed to bind socket");
		return false;
	}

	m_gameContext.reset(new Vanilla(this));

	//Register server
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


		sendServerInfoToMasterServer();
	}

	std::string serverAddr;
	enutil::toString(m_server->address, serverAddr);
	Logger::getInstance().info("Server", "Succesfully initialized server at " + serverAddr);
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
	enet_socket_destroy(m_socket);
	enet_deinitialize();
}

void Server::flushPackets()
{
	enet_host_flush(m_server);
}

const std::vector<std::unique_ptr<Peer>>& Server::getPeers()
{
	return m_peers;
}

Peer * Server::getPeer(int id)
{
	for (auto & p : m_peers)
		if (p->getId() == id)
			return p.get();
	return nullptr;
}

Peer * Server::getPeer(const ENetPeer * peer)
{
	for (auto & p : m_peers)
		if (p->getENetPeer() == peer)
			return p.get();
	return nullptr;
}


bool Server::ensurePlayers(Peer::State state)
{
	if (m_peers.empty())
		return false;
	for (auto & p : m_peers)
	{
		if (p->getState() != state)
			return false;
	}
	return true;
}

void Server::broadcast(const Packer & packer, bool reliable, const Peer * exclude)
{
	for (auto & p : m_peers)
	{
		if (p.get() != exclude)
		{
			p->send(packer, reliable);
		}
	}
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
			m_gameContext->prepareRound();
			m_state = LOADING;
			Packer packer;
			packer.pack(Msg::SV_LOAD_GAME);
			broadcast(packer, true);
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
			std::string peerString;
			enutil::toString(event.peer->address, peerString);
			Logger::getInstance().info("Server", peerString + " connected");
			enet_peer_timeout(event.peer, ENET_PEER_TIMEOUT_LIMIT, 500, 3000);
		}

		else if (event.type == ENET_EVENT_TYPE_RECEIVE)
		{
			Unpacker unpacker;
			enutil::receive(unpacker, event.packet);
			Msg msg;
			//unpacker.unpack(msg);

			unpacker.unpack(msg);
			if (msg == Msg::CL_REQUEST_JOIN_GAME)
			{
				std::string name;
				//unpacker.unpack(name);
				unpacker.unpack(name);

				Packer packer;
				if (m_state == PRE_GAME && m_peers.size() < 16)
				{
					packer.pack(Msg::SV_ACCEPT_JOIN);

					Peer * p = new Peer(m_nextPeerId++, event.peer);
					p->setName(name);
					m_peers.emplace_back(p);

					sendServerInfoToMasterServer();
				}
				else
				{
					packer.pack(Msg::SV_REJECT_JOIN);
				}
				enutil::send(packer, event.peer, true);
			}

			Peer * peer = getPeer(event.peer);
			if (!peer)
				return;


			if (msg == Msg::CL_REQUEST_ROOM_INFO)
			{

			}

			else if (msg == Msg::CL_REQUEST_PLAYER_INFO && m_state == LOADING)
			{
				peer->setState(Peer::LOADING);

				Packer packer;
				packer.pack(Msg::SV_PLAYER_INFO);
				packer.pack(m_peers.size());
				packer.pack(peer->getId());
				for (const auto & p : m_peers)
				{
					packer.pack(p->getId());
					packer.pack(p->getName());
				}

				peer->send(packer, true);
			}
			//Client will ask for info after the round has been asked to be prepared
			else if (msg == Msg::CL_REQUEST_GAME_INFO && m_state == LOADING)
			{
				peer->setState(Peer::LOADING);

				Packer packer;
				
				packer.pack(Msg::SV_GAME_INFO);
				packer.pack(m_gameContext->getMap().getName());				//map name
				
				peer->send(packer, true);
				

			}
			else if (msg == Msg::CL_LOAD_COMPLETE && m_state == LOADING)
			{
				peer->setState(Peer::IN_GAME);
				Logger::getInstance().info("GameContext", std::to_string(peer->getId()) + " has loaded");
				if (ensurePlayers(Peer::IN_GAME))
				{
					m_state = State::IN_GAME;
					Logger::getInstance().info("GameContext", "Everyone has loaded");
					m_gameContext->startRound();

					sendServerInfoToMasterServer();
				}
			}
			else if (msg == Msg::CL_INPUT && m_state == IN_GAME)
			{
				int predictedTick;
				int ackTick;
				unpacker.unpack(predictedTick);
				unpacker.unpack(ackTick);
				NetInput input;
				input.read(unpacker);
				peer->onInput(predictedTick, input);
				peer->setAckTick(ackTick);
				if (m_gameContext->getCurrentTick() % 2 == 0)
				{
					sf::Time timeLeft = sf::seconds(predictedTick / TICKS_PER_SEC) - m_gameContext->getCurrentTime();

					Packer packer;
					packer.pack(Msg::SV_INPUT_TIMING);
					packer.pack(predictedTick);
					packer.pack(timeLeft.asMilliseconds());
					peer->send(packer, false);
				}
			}
			else if (msg == Msg::CL_CHAT)
			{
				std::string chat;
				unpacker.unpack(chat);

				Packer packer;
				packer.pack(Msg::SV_CHAT);
				packer.pack(m_gameContext->getCurrentTick());
				packer.pack(peer->getId());
				packer.pack(chat);
				broadcast(packer, true);
			}

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
				std::string peerString;
				enutil::toString(event.peer->address, peerString);
				Logger::getInstance().info("Server", peerString + " disconnected");

				Peer * p = getPeer(event.peer);
				if (p)
				{
					Packer packer;
					packer.pack(Msg::SV_PLAYER_LEFT);
					packer.pack(p->getId());
					broadcast(packer, true, p);
					if (p->getEntity())
						p->getEntity()->setAlive(false);
				}
				auto pred = [p](const auto & ptr)
				{
					return ptr.get() == p;
				};
				m_peers.erase(std::remove_if(m_peers.begin(), m_peers.end(), pred), m_peers.end());

				if (m_peers.empty())
				{
					//m_nextPeerId = 0;
					reset();
				}
			}

			sendServerInfoToMasterServer();
		}
	}


	Unpacker unpacker;
	ENetAddress addr;
	while (enutil::receive(unpacker, addr, m_socket))
	{
		Msg msg;
		unpacker.unpack(msg);
		if (msg == Msg::CL_PING)
		{
			Packer packer;
			packer.pack(Msg::SV_PING_REPLY);
			enutil::send(packer, addr, m_socket);
		}
	}
}

void Server::update()
{
	static sf::Clock c;

	if (m_state == IN_GAME)
	{
		if (!m_gameContext->update())
			reset();
		if (c.getElapsedTime() >= sf::seconds(1))
		{
			std::cout << "out: " << m_server->totalSentData << "\n";
			m_server->totalSentData = 0;
			c.restart();
		}
	}
}

void Server::reset()
{
	Logger::getInstance().info("Server", "server reset!");

	m_state = PRE_GAME;
	for (auto & p : m_peers)
	{
		p->reset();

	}
	m_gameContext->reset();
}

void Server::sendServerInfoToMasterServer()
{
	//game: pingCheckPort, Name, mode, status, players

	ENetAddress pingCheckAddr;
	enet_socket_get_address(m_socket, &pingCheckAddr);

	Packer packer;
	packer.pack(Msg::SV_SERVER_INFO);
	packer.pack(pingCheckAddr.port);
	packer.pack(std::string("Fun game"));
	packer.pack(m_gameContext->getName());
	packer.pack(m_state);
	packer.pack(m_peers.size());
	enutil::send(packer, m_masterServer, true);
}
