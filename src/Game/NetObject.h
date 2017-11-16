#pragma once

#include "Enums.h"
#include "GameConfig.h"
#include "Core/Packer.h"
#include <SFML/System.hpp>
#include <functional>

struct NetObject
{
public:
	enum Type
	{
		NONE,
		HUMAN,
		ZOMBIE,
		PROJECTILE,
		CRATE,
		ITEM,
		PLAYER_INFO,
		ENTITY_COUNT,
		//Transient
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

	void writeRelative(Packer & packer, const NetObject & o) const;
	void readRelative(Unpacker & unpacker, const NetObject & o);
	NetObject * clone() const;

	std::vector<uint8_t> data;
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
	int airTick;
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
	sf::Vector2i vel;

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

struct NetPlayerInfo
{
	NetObject::Type getType() const;
	void write(Packer & packer) const;
	void read(Unpacker & unpacker);

	NetObject::Type type;
	int id;
	Team team;
	int ping;
	int score;
	int kills;
	int deaths;
	int assists;
	int respawnTick;

};

//Transient
struct NetExplosion
{
	NetObject::Type getType() const;
	void write(Packer & packer) const;
	void read(Unpacker & unpacker);

	sf::Vector2i pos;
};

