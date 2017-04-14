#include "GameCore.h"
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

CharacterCore * createCore(EntityType type)
{
	switch (type)
	{
	case EntityType::HUMAN:
		return new HumanCore;
		break;
	default:
		return nullptr;
	}
}

void HumanCore::tick(float dt, unsigned input, const Map & map)
{
	int direction = 0;
	if (input & Control::MOVE_LEFT)
		direction--;
	if (input & Control::MOVE_RIGHT)
		direction++;

	if (direction > 0)
		m_velocity.x = 600.f;
	else if (direction < 0)
		m_velocity.x = -600.f;
	else
		m_velocity.x = 0.f;


	Aabb<float> aabb(m_position.x, m_position.y, 50.f, 50.f);


	bool grounded = map.isGrounded(aabb);
	
	if (grounded)
	{
		if (input & Control::JUMP)
		{
			m_velocity.y = -1000.f;
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
	MoveResult result = map.move(aabb, m_velocity * dt);
	if (result.horizontalTile)
		m_velocity.x = 0.f;
	if (result.verticalTile)
		m_velocity.y = 0.f;
	if (result.verticalTile == -1)
		m_velocity.y = -1000.f;

	if(length(result.v) > 0.001f)
		m_position += result.v;
}

void HumanCore::assign(const NetObject * ne)
{
	const NetHuman * nh = static_cast<const NetHuman *>(ne);
	m_position.x = nh->pos.x / 100.f;
	m_position.y = nh->pos.y / 100.f;
	m_velocity.x = nh->vel.x / 100.f;
	m_velocity.y = nh->vel.y / 100.f;
}

CharacterCore * HumanCore::clone()
{
	HumanCore * h = new HumanCore();
	h->m_position = m_position;
	h->m_velocity = m_velocity;
	return h;
}
