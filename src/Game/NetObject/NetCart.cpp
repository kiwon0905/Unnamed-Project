#include "NetCart.h"

NetObject::Type NetCart::getType() const
{
	return NetObject::CART;
}

void NetCart::write(Packer & packer) const
{
	packer.pack(pos.x);
	packer.pack(pos.y);
}

void NetCart::read(Unpacker & unpacker)
{
	unpacker.unpack(pos.x);
	unpacker.unpack(pos.y);
}
