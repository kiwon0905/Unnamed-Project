#pragma once


#include "Core/Packet.h" 

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
		Connected,
		Disconnected,
		TimedOut,
		Received,
		None
	} type;
	std::unique_ptr<Packet> packet;
	NetEvent();
	NetEvent(NetEvent && netEvent);
};

class Network
{
public:
	bool initialize(Client & client);
	void finalize(Client & client);

	bool connect(const ENetAddress & addr);
	void disconnect();
	bool send(const Packet & packet, bool reliable);
	bool pollNetEvent(NetEvent & event);

	NetEvent * peekEvent();
	void popEvent();
	ENetSocket getSocket() { return m_socket; }
private:
	void hostService();
private:
	std::atomic<bool> m_running;
	std::unique_ptr<std::thread> m_serviceThread;
	std::mutex m_queueMutex;
	std::mutex m_clientMutex;
	ENetHost * m_client = nullptr;
	ENetSocket m_socket;
	ENetPeer * m_server = nullptr;
	std::deque<NetEvent> m_events;
	sf::Clock m_timeout;
	bool m_connecting = false;
};