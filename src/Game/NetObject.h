#pragma once

#include "GameConfig.h"
#include "Core/Packer.h"
#include <SFML/System.hpp>

struct NetObject
{
	virtual ~NetObject() {}
	enum Type
	{
		HUMAN,
		PROJECTILE,
		ENTITY_COUNT,
		COUNT
	};
	static NetObject * create(Type type);
	virtual void write(Packer & packer) const = 0;
	virtual void read(Unpacker & unpacker) = 0;
	virtual Type getType() const = 0;
};

//////////////////////////////////////////////////////////////

struct NetHuman : public NetObject
{
	void write(Packer & packer) const;
	void read(Unpacker & unpacker);
	Type getType() const;

	sf::Vector2i pos;
	sf::Vector2i vel;
	int aimAngle;
};

struct NetProjectile : public NetObject
{
	void write(Packer & packer) const;
	void read(Unpacker & unpacker);
	Type getType() const;
	sf::Vector2i pos;
};