#pragma once
#include "Core/Packer.h"
#include <SFML/System.hpp>

struct NetInput
{
	sf::Vector2f aimDirection;
	int moveDirection = 0;
	bool jump = false;
	bool fire = false;
	bool ability1 = false;
	bool ability2 = false;

	void write(Packer & packer);
	void read(Unpacker & unpacker);
};