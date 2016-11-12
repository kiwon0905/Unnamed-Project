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

bool Network::send(const Packet & packet, bool reliable)
{
	return enutil::send(m_server, packet, reliable) == 0;
}

bool Network::pollNetEvent(NetEvent & event)
{
	std::lock_guard<std::mutex> lock(m_queueMutex);
	if (m_events.size() == 0)
		return false;

	event.type = m_events.front().type;
	event.packet.reset(m_events.front().packet.release());
	m_events.pop_front();
	return true;
}

NetEvent * Network::peekEvent()
{
	if(m_events.size() == 0)
		return nullptr;
	return &m_events.front();
}

void Network::popEvent()
{
	std::lock_guard<std::mutex> lock(m_queueMutex);
	if (m_events.size())
		m_events.pop_front();
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
			NetEvent ev;
			if (event.type == ENET_EVENT_TYPE_CONNECT)
			{
				ev.type = NetEvent::Connected;
				enet_peer_timeout(event.peer, ENET_PEER_TIMEOUT_LIMIT, 500, 1000);
				m_connecting = false;
			}
			else if (event.type == ENET_EVENT_TYPE_DISCONNECT)
			{
				ev.type = NetEvent::Disconnected;
			}
			else if (event.type == ENET_EVENT_TYPE_RECEIVE)
			{
				ev.type = NetEvent::Received;
				ev.packet = std::make_unique<Packet>();
				ev.packet->write(event.packet->data, event.packet->dataLength);
				enet_packet_destroy(event.packet);
			}
			m_events.push_back(std::move(ev));
		}
		//Check if we are connecting
		if (m_connecting && m_timeout.getElapsedTime() > sf::seconds(5.f))
		{
			NetEvent ev;
			ev.type = NetEvent::TimedOut;
			m_events.push_back(std::move(ev));
			m_connecting = false;
			enet_peer_reset(m_server);
		}
		std::this_thread::yield();
	}
}

NetEvent::NetEvent():
	type(None)
{
}

NetEvent::NetEvent(NetEvent && netEvent):
	type(std::move(netEvent.type)),
	packet(std::move(netEvent.packet))
{
}
