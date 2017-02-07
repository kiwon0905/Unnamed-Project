#pragma once
#include <SFML/System.hpp>

class CharacterCore
{
public:
	virtual void update(float dt, unsigned input) = 0;
	const sf::Vector2f & getPosition() const;
	void setPosition(const sf::Vector2f & v);
protected:
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
};

class HumanCore : public CharacterCore
{
public:
	void update(float dt, unsigned input);
};