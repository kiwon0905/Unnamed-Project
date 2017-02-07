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



	m_gameServer = enet_host_create(&m_config.address, 32, 1, 0, 0);
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


	if (m_config.mode == "internet")
	{
		m_masterServer = enet_host_connect(m_gameServer, &m_config.masterAddress, 1, 1);
		if (!m_masterServer)
		{
			Logger::getInstance().error("Failed to connect to master server");
			return false;
		}
		Logger::getInstance().info("Connecting to master server...");
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

	Logger::getInstance().info("Succesfully initialized server at " + enutil::toString(m_gameServer->address));
	m_running = true;
	m_parsingThread.reset(new std::thread(&Server::parseCommands, this));
	return true;
}

void Server::run()
{
	if (initialize())
	{
		sf::Time elapsed = sf::Time::Zero;
		const sf::Time tickInterval = sf::seconds(1 / 60.f);

		while (m_running)
		{

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
					Msg msg;
					unpacker.unpack(msg);
					handlePacket(msg, unpacker, event.peer);
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
						Peer * peer = getPeer(event.peer);
						m_gameWorld.onDisconnect(*peer);
					}
					break;
				}
			}
			
			if(m_state > LOADING)
			{
				elapsed += m_clock.restart();

			

				while (elapsed >= tickInterval)
				{
					m_gameWorld.update(tickInterval.asSeconds(), m_players);
					elapsed -= tickInterval;
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
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
		else if (line == "start_dev")
		{
			if (m_players.size() > 0)
			{
				m_gameWorld.prepare(m_players);
				m_state = LOADING;
			}
		}
	}

}

void Server::handlePacket(Msg msg, Unpacker unpacker, ENetPeer * peer)
{
	if (msg == Msg::CL_REQUEST_JOIN_GAME)
	{
		Packer packer;
		if (m_state == PRE_GAME)
		{
			packer.pack(Msg::SV_ACCEPT_JOIN);
			
			Peer * p = new Peer(m_nextPeerId++, peer);
			m_players.emplace_back(p);
		}
		else
		{
			packer.pack(Msg::SV_REJECT_JOIN);
		}
		enutil::send(packer, peer, true);
		return;
	}

	Peer * p = getPeer(peer);
	if (!p)
		return;


	if (msg == Msg::CL_REQUEST_ROOM_INFO)
	{

	}
	else if (msg == Msg::CL_REQUEST_WORLD_INFO)
	{
		if (m_state == LOADING)
		{
			p->setState(Peer::LOADING);
			m_gameWorld.onRequestInfo(*p);
		}
	}
	else if (msg == Msg::CL_LOAD_COMPLETE)
	{
		if (m_state == LOADING)
		{
			p->setState(Peer::ENTERING);
			Logger::getInstance().info(std::to_string(p->getId()) + " has loaded");
			if (ensurePlayers(Peer::ENTERING))
			{
				m_state = State::ENTERING;
				m_clock.restart();
				Logger::getInstance().info("Everyone has loaded");
			}

		}
	}
	else if (msg == Msg::CL_READY)
	{
		p->setState(Peer::PLAYING);
		Logger::getInstance().info(std::to_string(p->getId()) + " has entered the game");
		if (ensurePlayers(Peer::PLAYING))
			m_gameWorld.start();
	}
	else if (msg == Msg::CL_INPUT)
	{
		std::uint8_t bits;
		int seq;
		unpacker.unpack<INPUT_SEQ_MIN, INPUT_SEQ_MAX>(seq);
		unpacker.unpack(bits);
		p->onInput(bits, seq);
	}

}

Peer * Server::getPeer(const ENetPeer * peer)
{
	for (auto & p : m_players)
		if (p->getENetPeer() == peer)
			return p.get();
	for (auto & p : m_spectators)
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
