#pragma once
#include <SFML/System.hpp>

#include "NetObject.h"
#include "Map.h"

class CharacterCore
{
public:
	virtual void tick(float dt, unsigned input, const Map & map) = 0;
	const sf::Vector2f & getPosition() const;
	void setPosition(const sf::Vector2f & v);
	const sf::Vector2f & getVelocity() const;
	virtual void assign(const NetItem * ne) = 0;
	virtual CharacterCore * clone() = 0;
protected:
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
};

CharacterCore * createCore(EntityType type);

class HumanCore : public CharacterCore
{
public:
	void tick(float dt, unsigned input, const Map & map);
	void assign(const NetItem * ne);
	CharacterCore * clone();

};


