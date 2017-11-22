#pragma once
#include "Game/NetObject.h"

struct NetProjectile
{
	NetObject::Type getType() const;
	void write(Packer & packer) const;
	void read(Unpacker & unpacker);

	ProjectileType type;
	sf::Vector2i pos;
	sf::Vector2i vel;
};