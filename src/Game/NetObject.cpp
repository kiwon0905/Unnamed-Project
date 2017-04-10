#include "NetObject.h"
#include "GameConfig.h"


int mround(float f)
{
	if (f > 0)
		return (int)(f + 0.5f);
	return (int)(f - 0.5f);
}
void NetHuman::write(Packer & packer) const
{
	packer.pack<2>(0.f, 5000.f, position.x);
	packer.pack<2>(0.f, 5000.f, position.y);
	packer.pack<2>(-5000.f, 5000.f, velocity.x);
	packer.pack<2>(-5000.f, 5000.f, velocity.y);
}

void NetHuman::read(Unpacker & unpacker)
{
	unpacker.unpack<2>(0.f, 5000.f, position.x);
	unpacker.unpack<2>(0.f, 5000.f, position.y);
	unpacker.unpack<2>(-5000.f, 5000.f, velocity.x);
	unpacker.unpack<2>(-5000.f, 5000.f, velocity.y);
}

NetItem::Type NetHuman::getType() const
{
	return Type::HUMAN;
}

NetItem * NetItem::create(Type type)
{
	switch (type)
	{
	case NetItem::HUMAN:
		return new NetHuman;
	default:
		return nullptr;
	}
}
