#include "NetObject.h"
#include "GameConfig.h"

NetObject * NetObject::create(Type type)
{
	switch (type)
	{
	case NetObject::HUMAN:
		return NetObject::create<NetHuman>();
	case NetObject::ZOMBIE:
		return NetObject::create<NetZombie>();
	case NetObject::PROJECTILE:
		return NetObject::create<NetProjectile>();
	case NetObject::CRATE:
		return NetObject::create<NetCrate>();
	case NetObject::EXPLOSION:
		return NetObject::create<NetExplosion>();
	default:
		return nullptr;
	}
}

NetObject::Type NetObject::getType() const
{
	return getTypeFunc(data.data());
}

void NetObject::write(Packer & packer) const
{
	writeFunc(data.data(), packer);
}

void NetObject::read(Unpacker & unpacker)
{
	readFunc(data.data(), unpacker);
}
NetObject * NetObject::xor(const NetObject & obj) const
{
	assert(getType() == obj.getType());

	NetObject * o = NetObject::create(getType());
	for (std::size_t i = 0; i < o->data.size(); ++i)
	{
		o->data[i] = data[i] ^ obj.data[i];
	}
	return o;
}
NetObject * NetObject::clone() const
{
	NetObject * o = NetObject::create(getType());
	o->data = data;
	return o;
}
//===================================================================//
NetObject::Type NetHuman::getType() const
{
	return NetObject::HUMAN;
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

NetObject::Type NetProjectile::getType() const
{
	return NetObject::PROJECTILE;
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

NetObject::Type NetZombie::getType() const
{
	return NetObject::ZOMBIE;
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

NetObject::Type NetExplosion::getType() const
{
	return NetObject::EXPLOSION;
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

NetObject::Type NetCrate::getType() const
{
	return NetObject::CRATE;
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

