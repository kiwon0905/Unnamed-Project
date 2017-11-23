#include "NetZombie.h"

NetObject::Type NetZombie::getType() const
{
	return NetObject::ZOMBIE;
}

void NetZombie::write(Packer & packer) const
{
	packer.pack(pos.x);
	packer.pack(pos.y);
	packer.pack(vel.x);
	packer.pack(vel.y);
	packer.pack(fuel);
	packer.pack(hoverCooldown);
	packer.pack(boostCooldown);
	packer.pack(refuelCooldown);
	packer.pack(aimAngle);
	packer.pack(health);
}

void NetZombie::read(Unpacker & unpacker)
{
	unpacker.unpack(pos.x);
	unpacker.unpack(pos.y);
	unpacker.unpack(vel.x);
	unpacker.unpack(vel.y);
	unpacker.unpack(fuel);
	unpacker.unpack(hoverCooldown);
	unpacker.unpack(boostCooldown);
	unpacker.unpack(refuelCooldown);
	unpacker.unpack(aimAngle);
	unpacker.unpack(health);
}
