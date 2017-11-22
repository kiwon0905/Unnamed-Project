#include "NetHuman.h"

NetObject::Type NetHuman::getType() const
{
	return NetObject::HUMAN;
}

void NetHuman::write(Packer & packer) const
{
	packer.pack(jump);
	packer.pack(airTick);
	packer.pack(health); //7
	packer.pack(pos.x); // 19
	packer.pack(pos.y); //19
	packer.pack(vel.x); //20
	packer.pack(vel.y); // 20
	packer.pack(aimAngle); // 9
}

void NetHuman::read(Unpacker & unpacker)
{
	unpacker.unpack(jump);
	unpacker.unpack(airTick);
	unpacker.unpack(health);
	unpacker.unpack(pos.x);
	unpacker.unpack(pos.y);
	unpacker.unpack(vel.x);
	unpacker.unpack(vel.y);
	unpacker.unpack(aimAngle);
}