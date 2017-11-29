#pragma once

#include "NetInput.h"
#include "NetObject/NetHuman.h"
#include "NetObject/NetZombie.h"
#include "Map.h"

#include <SFML/System.hpp>
#include <memory>

class CharacterCore;

class WorldCore
{
public:
	Map & getMap();
	struct CharacterInfo
	{
		Team team;
		int id;
		std::unique_ptr<CharacterCore> core;
	};
private:
	Map * m_map;

};

class CharacterCore
{
public:
	virtual void tick(float dt, const NetInput & input, const WorldCore & world) = 0;
	virtual CharacterCore * clone();

	const sf::Vector2f getPosition() const;
	void setPosition(const sf::Vector2f & position);

	const sf::Vector2f & getVelocity() const;
	void setVelocity(const sf::Vector2f & velocity);

	const sf::Vector2f & getSize() const;
	void setSize(const sf::Vector2f & size);
private:
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
	sf::Vector2f m_size;
};


class HumanCore
{
public:
	void tick(float dt, const NetInput & input, const Map & map);

	void read(const NetHuman & nh);
	void write(NetHuman & nh) const;

	const sf::Vector2f & getPosition() const;
	void setPosition(const sf::Vector2f & pos);
	const sf::Vector2f & getVelocity() const;
	void setVelocity(const sf::Vector2f & vel);
private:
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;

	int m_jump = 0;
	int m_airTick = 10;
};

class ZombieCore
{
public:
	void tick(float dt, const NetInput & input, const Map & map);

	void read(const NetZombie & nz);
	void write(NetZombie & nz) const;

	const sf::Vector2f & getPosition() const;
	void setPosition(const sf::Vector2f & pos);
private:
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
	int m_hoverCooldown = 0;
	int m_fuel = 100;
	int m_refuelCooldown = 0;
	int m_boostCooldown = 0;
};