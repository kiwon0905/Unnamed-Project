#pragma once


#include "Core/Packer.h" 
#include <SFML/System.hpp>
#include <deque>
#include <enet/enet.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>

constexpr unsigned MASTER_PORT = 12345;

class Client;

class Network
{
public:
	bool initialize(Client & client);
	void finalize(Client & client);

	void update();
	bool connect(const ENetAddress & addr);
	void disconnect();
	bool send(const Packer & packer, bool reliable);

	ENetEvent * peekEvent();
	void popEvent();
	bool send(Packer & packer, ENetAddress & addr);
	bool receive(Unpacker & unpacker, ENetAddress & addr);
private:
	ENetHost * m_client = nullptr;
	ENetSocket m_socket;
	ENetPeer * m_server = nullptr;
	std::deque<ENetEvent> m_events;
	bool m_connecting = false;
};