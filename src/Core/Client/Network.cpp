#include "Network.h"
#include "Core/ENetUtility.h"

#include <iostream>

bool Network::initialize(Client & client)
{
	if (enet_initialize() != 0)
		return false;

	m_client = enet_host_create(nullptr, 1, 1, 0, 0);
	if (!m_client)
		return false;
	if (enet_host_compress_with_range_coder(m_client) < 0)
		return false;
	m_socket = enet_socket_create(ENET_SOCKET_TYPE_DATAGRAM);
	if (m_socket < 0)
		return false;
	if (enet_socket_set_option(m_socket, ENET_SOCKOPT_NONBLOCK, 1) < 0)
		return false;
	if (enet_socket_set_option(m_socket, ENET_SOCKOPT_BROADCAST, 1) < 0)
		return false;

	m_running = true;
	m_serviceThread.reset(new std::thread(&Network::hostService, this));
	return true;
}

void Network::finalize(Client & client)
{
	m_running = false;
	if(m_server)
		enet_peer_disconnect_now(m_server, 0);
	if(m_serviceThread)
		m_serviceThread->join();
	if(m_client)
		enet_host_destroy(m_client);
	enet_socket_destroy(m_socket);
	enet_deinitialize();
}

//Connect to a server
bool Network::connect(const ENetAddress & addr)
{
	if (m_connecting)
		return false;
	disconnect();
	std::lock_guard<std::mutex> lock(m_clientMutex);	//protect m_client

	m_server = enet_host_connect(m_client, &addr, 2, 0);
	if (!m_server)
		return false;
	
	m_connecting = true;
	m_timeout.restart();
	return true;
}

void Network::disconnect()
{
	if (m_server)
	{
		enet_peer_disconnect(m_server, 0);
		m_server = nullptr;
	}
}

bool Network::send(const Packer & packer, bool reliable)
{
	return enutil::send(packer, m_server, reliable) == 0;
}

NetEvent * Network::peekEvent()
{
	if(m_events.size() == 0)
		return nullptr;
	return m_events.front().get();
}

void Network::popEvent()
{
	std::lock_guard<std::mutex> lock(m_queueMutex);
	if (m_events.size())
	{
		m_events.pop_front();
	}
}

bool Network::send(Packer & packer, ENetAddress & addr)
{
	return enutil::send(packer, addr, m_socket) == 0;
}

bool Network::receive(Unpacker & unpacker, ENetAddress & addr)
{
	return enutil::receive(unpacker, addr, m_socket) > 0;
}

void Network::hostService()
{
	ENetEvent event;
	while (m_running && m_client)
	{
		std::lock_guard<std::mutex> clientlock(m_clientMutex);
		std::lock_guard<std::mutex> queuelock(m_queueMutex);
		while (enet_host_service(m_client, &event, 0) > 0)
		{
			std::unique_ptr<NetEvent> ev(new NetEvent);
			if (event.type == ENET_EVENT_TYPE_CONNECT)
			{
				ev->type = NetEvent::Connected;
				enet_peer_timeout(event.peer, ENET_PEER_TIMEOUT_LIMIT, 500, 1000);
				m_connecting = false;
			}
			else if (event.type == ENET_EVENT_TYPE_DISCONNECT)
			{
				ev->type = NetEvent::Disconnected;
			}
			else if (event.type == ENET_EVENT_TYPE_RECEIVE)
			{
				ev->type = NetEvent::Received;
				ev->packet = event.packet;
				std::cout << "receivedlengthn: " << event.packet->dataLength << "\n";
			}
		
			m_events.push_back(std::move(ev));
		}
		//Check if we are connecting
		if (m_connecting && m_timeout.getElapsedTime() > sf::seconds(5.f))
		{
			std::unique_ptr<NetEvent> ev(new NetEvent);
			ev->type = NetEvent::TimedOut;
			m_events.push_back(std::move(ev));
			m_connecting = false;
			enet_peer_reset(m_server);
		}
		std::this_thread::yield();

	/*	for (auto & ev : m_events)
		{
			if (ev.packet)
				std::cout << ev.packet->dataLength << ", ";
		}
		std::cout << "\n";*/
	}
}

NetEvent::NetEvent() :
	type(None),
	packet(nullptr)
{
}

NetEvent::~NetEvent()
{
	enet_packet_destroy(packet);
}
