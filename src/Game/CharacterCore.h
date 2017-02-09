#pragma once
#include <SFML/System.hpp>

#include "NetObject.h"

class CharacterCore
{
public:
	virtual void update(float dt, unsigned input) = 0;
	const sf::Vector2f & getPosition() const;
	void setPosition(const sf::Vector2f & v);

	virtual void rollback(const NetEntity * ne) = 0;
protected:
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
};

class HumanCore : public CharacterCore
{
public:
	void update(float dt, unsigned input);
	void rollback(const NetEntity * ne);
};