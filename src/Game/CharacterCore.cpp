#include "CharacterCore.h"
#include "Control.h"

#include <iostream>
const sf::Vector2f & CharacterCore::getPosition() const
{
	return m_position;
}

void CharacterCore::setPosition(const sf::Vector2f & v)
{
	m_position = v;
}

void HumanCore::update(float dt, unsigned input)
{
	int direction = 0;
	if (input & Control::MOVE_LEFT)
		direction--;
	else if (input & Control::MOVE_RIGHT)
		direction++;
	
	if (direction > 0)
		m_velocity.x = 300.f;
	else if (direction < 0)
		m_velocity.x = -300.f;
	else
		m_velocity.x = 0.f;

	
	m_position += m_velocity * dt;
}
