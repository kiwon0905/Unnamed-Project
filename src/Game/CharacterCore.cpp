#include "CharacterCore.h"
#include "Control.h"
#include "Core/MathUtility.h"
#include <iostream>
const sf::Vector2f & CharacterCore::getPosition() const
{
	return m_position;
}

void CharacterCore::setPosition(const sf::Vector2f & v)
{
	m_position = v;
}

const sf::Vector2f & CharacterCore::getVelocity() const
{
	return m_velocity;
}

void HumanCore::update(float dt, unsigned input)
{
	int direction = 0;
	if (input & Control::MOVE_LEFT)
		direction--;
	if (input & Control::MOVE_RIGHT)
		direction++;
	
	if (direction > 0)
		m_velocity.x = 300.f;
	else if (direction < 0)
		m_velocity.x = -300.f;
	else
		m_velocity.x = 0.f;

	direction = 0;
	if (input & Control::MOVE_DOWN)
		direction--;
	if (input & Control::MOVE_UP)
		direction++;

	if (direction > 0)
		m_velocity.y = -300.f;
	else if (direction < 0)
		m_velocity.y = 300.f;
	else
		m_velocity.y = 0.f;

	
	m_position += m_velocity * dt;
}

void HumanCore::rollback(const NetEntity * ne, const CharacterCore * core)
{
	const NetHuman * nh = static_cast<const NetHuman *>(ne);
	const HumanCore * hc = static_cast<const HumanCore*>(core);

	
	if (hc)
		m_position = lerp(hc->m_position, nh->position, .1f);
	else
		m_position = nh->position;
	m_velocity = nh->velocity;
}

CharacterCore * HumanCore::clone()
{
	HumanCore * h = new HumanCore();
	h->m_position = m_position;
	return h;
}
