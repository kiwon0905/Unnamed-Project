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
	packer.pack_v(groundJump); // 1
	packer.pack_v(airJump);//1
	packer.pack_v(airTick);
	packer.pack_v(health); //7
	packer.pack_v(pos.x); // 19
	packer.pack_v(pos.y); //19
	packer.pack_v(vel.x); //20
	packer.pack_v(vel.y); // 20


	packer.pack_v(aimAngle); // 9
}

void NetHuman::read(Unpacker & unpacker)
{
	unpacker.unpack_v(groundJump);
	unpacker.unpack_v(airJump);
	unpacker.unpack_v(airTick);
	unpacker.unpack_v(health);
	unpacker.unpack_v(pos.x);
	unpacker.unpack_v(pos.y);
	unpacker.unpack_v(vel.x);
	unpacker.unpack_v(vel.y);


	unpacker.unpack_v(aimAngle);

}

NetObject::Type NetProjectile::getType() const
{
	return NetObject::PROJECTILE;
}

void NetProjectile::write(Packer & packer) const
{
	packer.pack_v(pos.x);
	packer.pack_v(pos.y);
	packer.pack_v(vel.x);
	packer.pack_v(vel.y);
}

void NetProjectile::read(Unpacker & unpacker)
{
	unpacker.unpack_v(pos.x);
	unpacker.unpack_v(pos.y);
	unpacker.unpack_v(vel.x);
	unpacker.unpack_v(vel.y);
}

NetObject::Type NetZombie::getType() const
{
	return NetObject::ZOMBIE;
}

void NetZombie::write(Packer & packer) const
{
	packer.pack_v(pos.x);
	packer.pack_v(pos.y);
	packer.pack_v(vel.x);
	packer.pack_v(vel.y);
}

void NetZombie::read(Unpacker & unpacker)
{
	unpacker.unpack_v(pos.x);
	unpacker.unpack_v(pos.y);
	unpacker.unpack_v(vel.x);
	unpacker.unpack_v(vel.y);
}

NetObject::Type NetCrate::getType() const
{
	return NetObject::CRATE;
}

void NetCrate::write(Packer & packer) const
{
	packer.pack_v(pos.x);
	packer.pack_v(pos.y);
}

void NetCrate::read(Unpacker & unpacker)
{
	unpacker.unpack_v(pos.x);
	unpacker.unpack_v(pos.y);
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
	packer.pack_v(type);
	packer.pack_v(id);
	packer.pack_v(score);
	packer.pack_v(team);

	packer.pack_v(kills);
	packer.pack_v(deaths);
	packer.pack_v(assists);

}

void NetPlayerInfo::read(Unpacker & unpacker)
{
	unpacker.unpack_v(type);
	unpacker.unpack_v(id);
	unpacker.unpack_v(score);
	unpacker.unpack_v(team);

	unpacker.unpack_v(kills);
	unpacker.unpack_v(deaths);
	unpacker.unpack_v(assists);
}

NetObject::Type NetExplosion::getType() const
{
	return NetObject::EXPLOSION;
}

void NetExplosion::write(Packer & packer) const
{
	packer.pack_v(pos.x);
	packer.pack_v(pos.y);
}

void NetExplosion::read(Unpacker & unpacker)
{
	unpacker.unpack_v(pos.x);
	unpacker.unpack_v(pos.y);
}