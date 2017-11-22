#include "NetProjectile.h"

NetObject::Type NetProjectile::getType() const
{
	return NetObject::PROJECTILE;
}

void NetProjectile::write(Packer & packer) const
{
	packer.pack(type);
	packer.pack(pos.x);
	packer.pack(pos.y);
	packer.pack(vel.x);
	packer.pack(vel.y);
}

void NetProjectile::read(Unpacker & unpacker)
{
	unpacker.unpack(type);
	unpacker.unpack(pos.x);
	unpacker.unpack(pos.y);
	unpacker.unpack(vel.x);
	unpacker.unpack(vel.y);
}