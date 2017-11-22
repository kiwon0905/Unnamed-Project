#pragma once
#include "Game/NetObject.h"

struct NetExplosion
{
	NetObject::Type getType() const;
	void write(Packer & packer) const;
	void read(Unpacker & unpacker);

	sf::Vector2i pos;
};

