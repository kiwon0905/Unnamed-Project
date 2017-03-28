#include "CharacterCore.h"
#include "Control.h"
#include "Core/Utility.h"
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


void HumanCore::update(float dt, unsigned input, const Map & map)
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

	//m_velocity.y += -10 * dt;	
	direction = 0;
	if (input & Control::MOVE_DOWN)
		direction--;
	if (input & Control::MOVE_UP)
		direction++;

	Aabb<float> aabb(m_position.x, m_position.y, 50.f, 50.f);


	bool grounded = map.isGrounded(aabb);
	
	if (grounded)
	{
		if (input & Control::JUMP)
		{
			m_velocity.y = -300.f;
		}
		else
		{
			m_velocity.y = 0.f;
		}
	}
	else
	{
		m_velocity.y += 1000.f * dt;
	}
	sf::Vector2f dv = map.move(aabb, m_velocity * dt);

	if(length(dv) > 0.001f)
		m_position += dv;
}

void HumanCore::assign(const NetEntity * ne)
{
	const NetHuman * nh = static_cast<const NetHuman *>(ne);
	m_position = nh->position;
	m_velocity = nh->velocity;
}

void HumanCore::smooth(const CharacterCore * core)
{
	const HumanCore * hc = static_cast<const HumanCore*>(core);
	m_velocity = hc->m_velocity;
	m_position = lerp(m_position, hc->m_position, .1f);
}

CharacterCore * HumanCore::clone()
{
	HumanCore * h = new HumanCore();
	h->m_position = m_position;
	return h;
}
