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
private:
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
	bool m_groundJump = false;
	bool m_airJump = false;
};

class ZombieCore
{
public:
	void tick(float dt, const NetInput & input, const Map & map);

	void read(const NetZombie & nz);
	void write(NetZombie & nz) const;

	const sf::Vector2f & getPosition() const;
private:
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
};