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
		ZOMBIE,
		PROJECTILE,
		ENTITY_COUNT,
		EXPLOSION,
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
	bool groundJump;
	bool airJump;
	int aimAngle;
	int health;
};

struct NetZombie : public NetObject
{
	void write(Packer & packer) const;
	void read(Unpacker & unpacker);
	Type getType() const;

	sf::Vector2i pos;
	sf::Vector2i vel;
};

struct NetProjectile : public NetObject
{
	void write(Packer & packer) const;
	void read(Unpacker & unpacker);
	Type getType() const;
	sf::Vector2i pos;
};

struct NetExplosion : public NetObject
{
	void write(Packer & packer) const;
	void read(Unpacker & unpacker);
	Type getType() const;
	sf::Vector2i pos;
};