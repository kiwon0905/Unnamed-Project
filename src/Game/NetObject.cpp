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
	packer.pack(jump);
	packer.pack(airTick);
	packer.pack(health); //7
	packer.pack(pos.x); // 19
	packer.pack(pos.y); //19
	packer.pack(vel.x); //20
	packer.pack(vel.y); // 20
	packer.pack(aimAngle); // 9
}

void NetHuman::read(Unpacker & unpacker)
{
	unpacker.unpack(jump);
	unpacker.unpack(airTick);
	unpacker.unpack(health);
	unpacker.unpack(pos.x);
	unpacker.unpack(pos.y);
	unpacker.unpack(vel.x);
	unpacker.unpack(vel.y);
	unpacker.unpack(aimAngle);
}

NetObject::Type NetProjectile::getType() const
{
	return NetObject::PROJECTILE;
}

void NetProjectile::write(Packer & packer) const
{
	packer.pack(pos.x);
	packer.pack(pos.y);
	packer.pack(vel.x);
	packer.pack(vel.y);
}

void NetProjectile::read(Unpacker & unpacker)
{
	unpacker.unpack(pos.x);
	unpacker.unpack(pos.y);
	unpacker.unpack(vel.x);
	unpacker.unpack(vel.y);
}

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
}

void NetZombie::read(Unpacker & unpacker)
{
	unpacker.unpack(pos.x);
	unpacker.unpack(pos.y);
	unpacker.unpack(vel.x);
	unpacker.unpack(vel.y);
}

NetObject::Type NetCrate::getType() const
{
	return NetObject::CRATE;
}

void NetCrate::write(Packer & packer) const
{
	packer.pack(pos.x);
	packer.pack(pos.y);
}

void NetCrate::read(Unpacker & unpacker)
{
	unpacker.unpack(pos.x);
	unpacker.unpack(pos.y);
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
	packer.pack(id);
	packer.pack(team);
	packer.pack(ping);
	packer.pack(score);
	packer.pack(kills);
	packer.pack(deaths);
	packer.pack(assists);
	packer.pack(respawnTick);
}

void NetPlayerInfo::read(Unpacker & unpacker)
{
	unpacker.unpack(type);
	unpacker.unpack(id);
	unpacker.unpack(team);
	unpacker.unpack(ping);
	unpacker.unpack(score);
	unpacker.unpack(kills);
	unpacker.unpack(deaths);
	unpacker.unpack(assists);
	unpacker.unpack(respawnTick);
}

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