#include "GameCore.h"
#include "Core/Utility.h"


void HumanCore::tick(float dt, const NetInput & input, const Map & map)
{
	Aabb<float> aabb(m_position.x, m_position.y, 69.f, 69.f);
	bool grounded = map.isGrounded(aabb);

	float accel = 10000.f;
	float friction = .5f;
	float maxSpeed = 500.f;

	if (!grounded)
	{
		accel = 3000.f;
		friction = .99f;
		//maxSpeed = 1400.f;
	}

	m_velocity.y = clampedAdd(-3000.f, 3000.f, m_velocity.y, 1000.f * dt);

	if (input.moveDirection > 0)
		m_velocity.x = clampedAdd(-maxSpeed, maxSpeed, m_velocity.x, accel * dt);
	else if (input.moveDirection < 0)
		m_velocity.x = clampedAdd(-maxSpeed, maxSpeed, m_velocity.x, -accel * dt);
	else
		m_velocity.x = m_velocity.x * friction;

	if (grounded)
	{
		if (input.jump)
		{
			m_velocity.y = -700.f;
		}
	}
	else
	{
	}

	MoveResult result = map.move(aabb, m_velocity * dt);

	if (result.horizontalTile)
		m_velocity.x = 0.f;
	if (result.verticalTile)
		m_velocity.y = 0.f;

	if (length(result.v) > 0.001f)
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

const sf::Vector2f & HumanCore::getPosition() const
{
	return m_position;
}

const sf::Vector2f & HumanCore::getVelocity() const
{
	return m_velocity;
}
