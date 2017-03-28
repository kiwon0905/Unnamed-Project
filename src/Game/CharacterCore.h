#pragma once
#include <SFML/System.hpp>

#include "NetObject.h"
#include "Map.h"

class CharacterCore
{
public:
	virtual void update(float dt, unsigned input, const Map & map) = 0;
	const sf::Vector2f & getPosition() const;
	void setPosition(const sf::Vector2f & v);
	const sf::Vector2f & getVelocity() const;
	virtual void assign(const NetEntity * ne) = 0;
	virtual void smooth(const CharacterCore * core) = 0;
	virtual CharacterCore * clone() = 0;
protected:
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
};

class HumanCore : public CharacterCore
{
public:
	void update(float dt, unsigned input, const Map & map);
	void assign(const NetEntity * ne);
	void smooth(const CharacterCore * core);
	CharacterCore * clone();

};