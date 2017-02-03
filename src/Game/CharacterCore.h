#pragma once
#include <SFML/System.hpp>


class CharacterCore
{
public:
	void update(float dt);
private:
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
};