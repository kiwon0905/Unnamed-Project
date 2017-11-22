#include "NetItem.h"

NetObject::Type NetItem::getType() const
{
	return NetObject::ITEM;
}

void NetItem::write(Packer & packer) const
{
}

void NetItem::read(Unpacker & unpacker)
{
}
