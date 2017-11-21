#pragma once
#include <SFML/System.hpp>

#include "NetInput.h"
#include "NetObject.h"
#include "Map.h"

class WorldCore
{
public:

	virtual Map & getMap() = 0;
private:
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
	int m_fuel;
	int m_airTick;
};