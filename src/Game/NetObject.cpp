#include "NetObject.h"
#include "GameConfig.h"

#include "NetObject/NetHuman.h"
#include "NetObject/NetZombie.h"
#include "NetObject/NetProjectile.h"
#include "NetObject/NetPlayerInfo.h"
#include "NetObject/NetExplosion.h"
#include "NetObject/NetGameDataControl.h"
#include "NetObject/NetGameDataTdm.h"

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
	case NetObject::PLAYER_INFO:
		return NetObject::create<NetPlayerInfo>();
	case NetObject::EXPLOSION:
		return NetObject::create<NetExplosion>();
	case NetObject::GAME_DATA_CONTROL:
		return NetObject::create<NetGameDataControl>();
	case NetObject::GAME_DATA_TDM:
		return NetObject::create<NetGameDataTdm>();
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


