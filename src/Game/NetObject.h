#pragma once

#include "GameCore.h"
#include "Core/Packer.h"
#include <SFML/System.hpp>

struct NetEntity
{
	virtual void pack(Packer & packer) = 0;
	virtual void unpack(Unpacker & unpacker) = 0;
	virtual EntityType getType() = 0;
};

struct NetHuman : public NetEntity
{
	void pack(Packer & packer);
	void unpack(Unpacker & unpacker);
	EntityType getType();
	sf::Vector2f position;
};