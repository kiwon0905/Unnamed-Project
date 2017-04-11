#include "NetObject.h"
#include "GameConfig.h"

NetObject * NetObject::create(Type type)
{
	switch (type)
	{
	case NetObject::HUMAN:
		return new NetHuman;
	case NetObject::PROJECTILE:
		return new NetProjectile;
	default:
		return nullptr;
	}
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

NetObject::Type NetHuman::getType() const
{
	return Type::HUMAN;
}

void NetProjectile::write(Packer & packer) const
{
	packer.pack<2>(0.f, 5000.f, position.x);
	packer.pack<2>(0.f, 5000.f, position.y);
}

void NetProjectile::read(Unpacker & unpacker)
{
	unpacker.unpack<2>(0.f, 5000.f, position.x);
	unpacker.unpack<2>(0.f, 5000.f, position.y);
}

NetObject::Type NetProjectile::getType() const
{
	return NetObject::PROJECTILE;
}
