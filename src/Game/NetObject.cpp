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
	case NetObject::PLAYER_INFO:
		return NetObject::create<NetPlayerInfo>();
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

void NetObject::writeRelative(Packer & packer, const NetObject & o) const
{
	std::vector<char> xord(data.size());
	for (std::size_t i = 0; i < xord.size(); ++i)
	{
		xord[i] = data[i] ^ o.data[i];
	}

	writeFunc(xord.data(), packer);

}

void NetObject::readRelative(Unpacker & unpacker, const NetObject & o)
{
	readFunc(data.data(), unpacker);

	for (std::size_t i = 0; i < data.size(); ++i)
	{
		data[i] = data[i] ^ o.data[i];
	}

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
	packer.pack(groundJump); // 1
	packer.pack(airJump);//1
	packer.pack<-1, MAX_TICK>(airTick);
	packer.pack<0, 100>(health); //7
	packer.pack<0, 500000>(pos.x); // 19
	packer.pack<0, 500000>(pos.y); //19
	packer.pack<-500000, 500000>(vel.x); //20
	packer.pack<-500000, 500000>(vel.y); // 20


	packer.pack<0, 360>(aimAngle); // 9
}

void NetHuman::read(Unpacker & unpacker)
{
	unpacker.unpack(groundJump);
	unpacker.unpack(airJump);
	unpacker.unpack<-1, MAX_TICK>(airTick);
	unpacker.unpack<0, 100>(health);
	unpacker.unpack<0, 500000>(pos.x);
	unpacker.unpack<0, 500000>(pos.y);
	unpacker.unpack<-500000, 500000>(vel.x);
	unpacker.unpack<-500000, 500000>(vel.y);


	unpacker.unpack<0, 360>(aimAngle);

}

NetObject::Type NetProjectile::getType() const
{
	return NetObject::PROJECTILE;
}

void NetProjectile::write(Packer & packer) const
{
	packer.pack<0, 500000>(pos.x);
	packer.pack<0, 500000>(pos.y);
	packer.pack<-500000, 500000>(vel.x);
	packer.pack<-500000, 500000>(vel.y);
}

void NetProjectile::read(Unpacker & unpacker)
{
	unpacker.unpack<0, 500000>(pos.x);
	unpacker.unpack<0, 500000>(pos.y);
	unpacker.unpack<-500000, 500000>(vel.x);
	unpacker.unpack<-500000, 500000>(vel.y);
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

NetObject::Type NetPlayerInfo::getType() const
{
	return NetObject::PLAYER_INFO;
}

void NetPlayerInfo::write(Packer & packer) const
{
	packer.pack(type);
	packer.pack<0, MAX_ENTITY_ID>(id);
	packer.pack<0, 1023>(score);
	packer.pack(team);
}

void NetPlayerInfo::read(Unpacker & unpacker)
{
	unpacker.unpack(type);
	unpacker.unpack<0, MAX_ENTITY_ID>(id);
	unpacker.unpack<0, 1023>(score);
	unpacker.unpack(team);
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