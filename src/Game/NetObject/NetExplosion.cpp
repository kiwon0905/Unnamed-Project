#include "NetExplosion.h"

NetObject::Type NetExplosion::getType() const
{
	return NetObject::EXPLOSION;
}

void NetExplosion::write(Packer & packer) const
{
	packer.pack(pos.x);
	packer.pack(pos.y);
}

void NetExplosion::read(Unpacker & unpacker)
{
	unpacker.unpack(pos.x);
	unpacker.unpack(pos.y);
}