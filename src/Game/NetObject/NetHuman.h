#pragma once
#include "Game/NetObject.h"

struct NetHuman
{
	NetObject::Type getType() const;
	void write(Packer & packer) const;
	void read(Unpacker & unpacker);

	sf::Vector2i pos;
	sf::Vector2i vel;
	int jump;
	int airTick;
	///
	int aimAngle;
	int health;
};