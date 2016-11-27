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
struct NetEvent
{
	enum Type
	{
		CONNECTED,
		DISCONNECTED,
		TIMED_OUT,
		RECEIVED,
		NONE
	} type;
	ENetPacket * packet;
	NetEvent();
	~NetEvent();
};

class Network
{
public:
	bool initialize(Client & client);
	void finalize(Client & client);

	void update();
	bool connect(const ENetAddress & addr);
	void disconnect();
	bool send(const Packer & packer, bool reliable);

	NetEvent * peekEvent();
	void popEvent();
	bool send(Packer & packer, ENetAddress & addr);
	bool receive(Unpacker & unpacker, ENetAddress & addr);
private:
	ENetHost * m_client = nullptr;
	ENetSocket m_socket;
	ENetPeer * m_server = nullptr;
	std::deque<std::unique_ptr<NetEvent>> m_events;
	sf::Clock m_timeout;
	bool m_connecting = false;
};