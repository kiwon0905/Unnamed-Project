#pragma once
#include <enet/enet.h>
#include <string>
#include "Packer.h"

namespace enutil
{
	std::string toString(const ENetAddress & address);
	ENetAddress toENetAddress(const std::string & addr);
	ENetAddress toENetAddress(const std::string & ip, unsigned port);

	int send(const Packer & packer, ENetPeer * peer, bool reliable);
	int receive(Unpacker & unpacker, ENetPacket * p);
	int send(const Packer & packer, const ENetAddress & addr, ENetSocket socket);
	int receive(Unpacker & packer, ENetAddress & addr, ENetSocket socket);
}

