#include "GameCore.h"
#include "Control.h"
#include "Core/Utility.h"


void HumanCore::tick(float dt, unsigned input, const Map & map)
{
	int direction = 0;
	if (input & Control::MOVE_LEFT)
		direction--;
	if (input & Control::MOVE_RIGHT)
		direction++;

	if (direction > 0)
		m_velocity.x = 600;// clampedAdd(-600.f, 600.f, m_velocity.x, 3200 * dt);
	else if (direction < 0)
		m_velocity.x = -600;// clampedAdd(-600.f, 600.f, m_velocity.x, -3200.f * dt);
	else
		m_velocity.x = m_velocity.x * .5f;

	Aabb<float> aabb(m_position.x, m_position.y, 70.f, 70.f);
	
	bool grounded = map.isGrounded(aabb);

	if (grounded)
	{
		if (input & Control::JUMP)
		{
			m_velocity.y = -700.f;
		}
		else
		{
			m_velocity.y = 0.f;
		}
	}
	else
	{
		m_velocity.y += 800.f * dt;
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
