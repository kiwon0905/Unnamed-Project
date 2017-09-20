#pragma once
#include "Core/Packer.h"
#include <SFML/System.hpp>

struct NetInput
{
	//int tick = 0;

	sf::Vector2f aimDirection;
	int  moveDirection = 0;
	int vMoveDirection = 0;
	bool jump = false;
	bool fire = false;

	void write(Packer & packer);
	void read(Unpacker & unpacker);
};