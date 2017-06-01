#pragma once
#include "Core/Packer.h"
#include <SFML/System.hpp>

enum Control
{
	JUMP = 1 << 0,
	MOVE_LEFT = 1 << 1,
	MOVE_RIGHT = 1 << 2,
	MOVE_UP = 1 << 3,
	MOVE_DOWN = 1 << 4,
	PRIMARY_FIRE = 1 << 5,
	ABILITY1 = 1 << 6,
	ABILITY2 = 1 << 7,
};

struct NetInput
{
	sf::Vector2f aimDirection;
	int  moveDirection;
	bool jump;
	bool fire;

	void write(Packer & packer);
	void read(Unpacker & unpacker);
};