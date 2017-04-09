#pragma once

#include "GameConfig.h"
#include "Core/Packer.h"
#include <SFML/System.hpp>

struct NetItem
{
	virtual ~NetItem() {}
	enum Type
	{
		HUMAN,
		COUNT
	};
	static NetItem * create(Type type);
	virtual void write(Packer & packer) const = 0;
	virtual void read(Unpacker & unpacker) = 0;
	virtual Type getType() const = 0;
};

//////////////////////////////////////////////////////////////

struct NetHuman : public NetItem
{
	void write(Packer & packer) const;
	void read(Unpacker & unpacker);
	Type getType() const;
	sf::Vector2f position;
	sf::Vector2f velocity;
};

