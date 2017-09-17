#pragma once

#include "GameConfig.h"
#include "Core/Packer.h"
#include <SFML/System.hpp>
#include <functional>

struct NetObject
{
public:
	enum Type
	{
		HUMAN,
		ZOMBIE,
		PROJECTILE,
		CRATE,
		ITEM,
		ENTITY_COUNT,
		EXPLOSION,
		COUNT
	};

private:
	NetObject() {}
public:
	static NetObject * create(Type type);
	template <typename T>
	static NetObject * create();

	Type getType() const;
	void write(Packer & packer) const;
	void read(Unpacker & unpacker);

	void writeRelative(Packer & packer, const NetObject & o);
	void readRelative(Unpacker & unpacker, const NetObject & o);
	NetObject * clone() const;

	std::vector<char> data;
	Type(*getTypeFunc)(const void *);
	void(*writeFunc)(const void *, Packer &);
	void(*readFunc)(void *, Unpacker &);
};

template <typename T>
NetObject * NetObject::create()
{
	NetObject * o = new NetObject;
	o->data.resize(sizeof(T), 0);

	o->getTypeFunc = [](const void * ptr)
	{
		return std::bind(&T::getType, static_cast<const T*>(ptr))();
	};
	o->writeFunc = [](const void * ptr, Packer & packer)
	{
		std::bind(&T::write, static_cast<const T*>(ptr), std::ref(packer))();
	};
	o->readFunc = [](void * ptr, Unpacker & unpacker)
	{
		std::bind(&T::read, static_cast<T*>(ptr), std::ref(unpacker))();
	};
	return o;
}

//////////////////////////////////////////////////////////////

struct NetHuman
{
	NetObject::Type getType() const;
	void write(Packer & packer) const;
	void read(Unpacker & unpacker);

	sf::Vector2i pos;
	sf::Vector2i vel;
	bool groundJump;
	bool airJump;
	///
	int aimAngle;
	int health;
};

struct NetZombie
{
	NetObject::Type getType() const;
	void write(Packer & packer) const;
	void read(Unpacker & unpacker);

	sf::Vector2i pos;
	sf::Vector2i vel;
};

struct NetProjectile
{
	NetObject::Type getType() const;
	void write(Packer & packer) const;
	void read(Unpacker & unpacker);

	sf::Vector2i pos;
};

struct NetCrate
{
	NetObject::Type getType() const;
	void write(Packer & packer) const;
	void read(Unpacker & unpacker);

	sf::Vector2i pos;
};

struct NetItem
{
	NetObject::Type getType() const;
	void write(Packer & packer) const;
	void read(Unpacker & unpacker);

	sf::Vector2i pos;
};

//Transient
struct NetExplosion
{
	NetObject::Type getType() const;
	void write(Packer & packer) const;
	void read(Unpacker & unpacker);

	sf::Vector2i pos;
};