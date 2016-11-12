#pragma once
#include <enet/enet.h>
#include <string>
#include "Packet.h"

namespace enutil
{
	std::string toString(const ENetAddress & address);
	ENetAddress toENetAddress(const std::string & addr);
	ENetAddress toENetAddress(const std::string & ip, unsigned port);
	int send(ENetPeer * peer, const Packet & packet, bool reliable);
	void receive(ENetPacket * p, Packet & packet);
	int send(ENetSocket socket, const ENetAddress & addr, const Packet & packet);
	int receive(ENetSocket socket, ENetAddress & addr, Packet & packet);
}

