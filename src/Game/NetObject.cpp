#include "NetObject.h"
#include "GameConfig.h"

NetObject * NetObject::create(Type type)
{
	switch (type)
	{
	case NetObject::HUMAN:
		return new NetHuman;
	case NetObject::ZOMBIE:
		return new NetZombie;
	case NetObject::PROJECTILE:
		return new NetProjectile;
	case NetObject::CRATE:
		return new NetCrate;
	case NetObject::EXPLOSION:
		return new NetExplosion;
	default:
		return nullptr;
	}
}

void NetHuman::write(Packer & packer) const
{
	packer.pack<0, 500000>(pos.x);
	packer.pack<0, 500000>(pos.y);
	packer.pack<-500000, 500000>(vel.x);
	packer.pack<-500000, 500000>(vel.y);
	packer.pack<0, 360>(aimAngle);
	packer.pack(groundJump);
	packer.pack(airJump);
	packer.pack<0, 100>(health);
}

void NetHuman::read(Unpacker & unpacker)
{
	unpacker.unpack<0, 500000>(pos.x);
	unpacker.unpack<0, 500000>(pos.y);
	unpacker.unpack<-500000, 500000>(vel.x);
	unpacker.unpack<-500000, 500000>(vel.y);
	unpacker.unpack<0, 360>(aimAngle);
	unpacker.unpack(groundJump);
	unpacker.unpack(airJump);
	unpacker.unpack<0, 100>(health);
}

NetObject::Type NetHuman::getType() const
{
	return Type::HUMAN;
}

void NetProjectile::write(Packer & packer) const
{
	packer.pack<0, 500000>(pos.x);
	packer.pack<0, 500000>(pos.y);
}

void NetProjectile::read(Unpacker & unpacker)
{
	unpacker.unpack<0, 500000>(pos.x);
	unpacker.unpack<0, 500000>(pos.y);
}

NetObject::Type NetProjectile::getType() const
{
	return NetObject::PROJECTILE;
}

void NetZombie::write(Packer & packer) const
{
	packer.pack<0, 500000>(pos.x);
	packer.pack<0, 500000>(pos.y);
	packer.pack<-500000, 500000>(vel.x);
	packer.pack<-500000, 500000>(vel.y);
}

void NetZombie::read(Unpacker & unpacker)
{
	unpacker.unpack<0, 500000>(pos.x);
	unpacker.unpack<0, 500000>(pos.y);
	unpacker.unpack<-500000, 500000>(vel.x);
	unpacker.unpack<-500000, 500000>(vel.y);
}

NetObject::Type NetZombie::getType() const
{
	return Type::ZOMBIE;
}

void NetExplosion::write(Packer & packer) const
{
	packer.pack<0, 500000>(pos.x);
	packer.pack<0, 500000>(pos.y);
}

void NetExplosion::read(Unpacker & unpacker)
{
	unpacker.unpack<0, 500000>(pos.x);
	unpacker.unpack<0, 500000>(pos.y);
}

NetExplosion::Type NetExplosion::getType() const
{
	return NetObject::EXPLOSION;
}

void NetCrate::write(Packer & packer) const
{
	packer.pack<0, 500000>(pos.x);
	packer.pack<0, 500000>(pos.y);
}

void NetCrate::read(Unpacker & unpacker)
{
	unpacker.unpack<0, 500000>(pos.x);
	unpacker.unpack<0, 500000>(pos.y);
}

NetObject::Type NetCrate::getType() const
{
	return NetObject::CRATE;
}

void NetItem::write(Packer & packer) const
{
}

void NetItem::read(Unpacker & unpacker)
{
}

NetObject::Type NetItem::getType() const
{
	return Type();
}
