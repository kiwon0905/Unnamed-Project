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
	void assign(const NetObject * ne);

	const sf::Vector2f & getPosition() const;
	const sf::Vector2f & getVelocity() const;
private:
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
};

class ZombieCore
{
public:
	void tick(float dt, const NetInput & input, const Map & map);
	void assign(const NetObject * ne);

	const sf::Vector2f & getPosition() const;
	const sf::Vector2f & getVelocity() const;
private:
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
};