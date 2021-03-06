#pragma once
#include "Game/NetObject.h"

struct NetZombie
{
	NetObject::Type getType() const;
	void write(Packer & packer) const;
	void read(Unpacker & unpacker);

	sf::Vector2i pos;
	sf::Vector2i vel;
	int fuel;
	int hoverCooldown;
	int boostCooldown;
	int refuelCooldown;
	int aimAngle;
	int health;
};




