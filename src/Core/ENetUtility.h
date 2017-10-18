#pragma once
#include <enet/enet.h>
#include <string>
#include "Packer.h"

namespace enutil
{
	bool toString(const ENetAddress & address, std::string & ans);
	bool toENetAddress(const std::string & addr, ENetAddress & ans);
	bool toENetAddress(const std::string & ip, unsigned port, ENetAddress & ans);

	bool send(const Packer & packer, ENetPeer * peer, bool reliable);
	bool receive(Unpacker & unpacker, ENetPacket * p);
	bool send(const Packer & packer, const ENetAddress & addr, ENetSocket socket);
	bool receive(Unpacker & packer, ENetAddress & addr, ENetSocket socket);
}

