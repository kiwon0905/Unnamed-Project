#include "NetPlayerInfo.h"

NetObject::Type NetPlayerInfo::getType() const
{
	return NetObject::PLAYER_INFO;
}

void NetPlayerInfo::write(Packer & packer) const
{
	packer.pack(type);
	packer.pack(id);
	packer.pack(team);
	packer.pack(ping);
	packer.pack(score);
	packer.pack(kills);
	packer.pack(deaths);
	packer.pack(assists);
	packer.pack(respawnTick);
}

void NetPlayerInfo::read(Unpacker & unpacker)
{
	unpacker.unpack(type);
	unpacker.unpack(id);
	unpacker.unpack(team);
	unpacker.unpack(ping);
	unpacker.unpack(score);
	unpacker.unpack(kills);
	unpacker.unpack(deaths);
	unpacker.unpack(assists);
	unpacker.unpack(respawnTick);
}

