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
	return true;
}

void Network::finalize(Client & client)
{
	if(m_server)
		enet_peer_disconnect_now(m_server, 0);
	if(m_client)
		enet_host_destroy(m_client);
	enet_socket_destroy(m_socket);
	enet_deinitialize();
}

void Network::update()
{
	ENetEvent event;

	while (enet_host_service(m_client, &event, 0) > 0)
	{
		if (event.type != ENET_EVENT_TYPE_NONE)
			m_events.push_back(event);
		if (event.type == ENET_EVENT_TYPE_DISCONNECT)
		{
			enet_peer_reset(event.peer);
			m_connecting = false;
		}
		if (event.type == ENET_EVENT_TYPE_CONNECT)
		{
			m_connecting = false;
		}
	}
}

//Connect to a server
bool Network::connect(const ENetAddress & addr)
{
	if (m_connecting)
		return false;
	disconnect();

	m_server = enet_host_connect(m_client, &addr, 2, 0);

	if (!m_server)
		return false;
	enet_peer_timeout(m_server, ENET_PEER_TIMEOUT_LIMIT, 500, 3000);
	m_connecting = true;
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

ENetEvent * Network::peekEvent()
{
	if(m_events.size() == 0)
		return nullptr;
	return &m_events.front();
}

void Network::popEvent()
{
	if (m_events.size())
	{
		enet_packet_destroy(m_events.front().packet);
		m_events.pop_front();
	}
}

bool Network::send(const Packer & packer, const ENetAddress & addr)
{
	return enutil::send(packer, addr, m_socket);
}

bool Network::receive(Unpacker & unpacker, ENetAddress & addr)
{
	return enutil::receive(unpacker, addr, m_socket);
}
