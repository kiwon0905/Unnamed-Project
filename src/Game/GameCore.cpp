#include "GameCore.h"
#include "Core/Utility.h"


void HumanCore::tick(float dt, const NetInput & input, const Map & map)
{
	Aabb aabb(m_position.x, m_position.y, 69.f, 69.f);
	bool grounded = map.isGrounded(aabb);

	float accel = 10000.f;
	float friction = .5f;
	float maxSpeed = 600.f;

	m_velocity.y = Math::clampedAdd(-3000.f, 3000.f, m_velocity.y, 1300.f * dt);

	if (!grounded)
	{
		accel = 3000.f;
		friction = .99f;
		//maxSpeed = 1400.f;
	}
	


	if (input.moveDirection > 0)
		m_velocity.x = Math::clampedAdd(-maxSpeed, maxSpeed, m_velocity.x, accel * dt);
	else if (input.moveDirection < 0)
		m_velocity.x = Math::clampedAdd(-maxSpeed, maxSpeed, m_velocity.x, -accel * dt);
	else
		m_velocity.x = m_velocity.x * friction;


	if (grounded)
	{
		m_airJump = false;
		m_groundJump = false;
		m_airTick = 0;
	}
	else
	{
		m_airTick++;
	}

	if (input.jump)
	{
		if (grounded || m_airTick < 10)
		{
			if (!m_groundJump)
			{
				m_velocity.y = -700.f;
				m_groundJump = true;
			}
		}
		else
		{
			if (!m_airJump)
			{
				m_airJump = true;
				m_velocity.y = -650.f;
			}
		}
	}

	sf::Vector2f d = m_velocity * dt;

	
	sf::Vector2f out;
	sf::Vector2i norm;
	sf::Vector2i tile;
	if (map.sweepCharacter(aabb, d, out, norm, tile))
	{
		if (norm.x)
			m_velocity.x = 0.f;
		if (norm.y)
			m_velocity.y = 0.f;
		d = out;
	}
	

	m_position += d;
}

void HumanCore::read(const NetHuman & nh)
{
	m_position.x = nh.pos.x / 100.f;
	m_position.y = nh.pos.y / 100.f;
	m_velocity.x = nh.vel.x / 100.f;
	m_velocity.y = nh.vel.y / 100.f;
	m_airJump = nh.airJump;
	m_groundJump = nh.groundJump;
	m_airTick = nh.airTick;
}

void HumanCore::write(NetHuman & nh) const
{
	nh.vel.x = Math::roundToInt(m_velocity.x * 100.f);
	nh.vel.y = Math::roundToInt(m_velocity.y * 100.f);
	nh.pos.x = Math::roundToInt(m_position.x * 100.f);
	nh.pos.y = Math::roundToInt(m_position.y * 100.f);
	nh.groundJump = m_groundJump;
	nh.airJump = m_airJump;
	nh.airTick = m_airTick;
}

const sf::Vector2f & HumanCore::getPosition() const
{
	return m_position;
}

void HumanCore::setPosition(const sf::Vector2f & pos)
{
	m_position = pos;
}

void ZombieCore::tick(float dt, const NetInput & input, const Map & map)
{
	Aabb aabb(m_position.x, m_position.y, 69.f, 69.f);
	bool grounded = map.isGrounded(aabb);

	float accel = 10000.f;
	float friction = .5f;
	float maxSpeed = 600.f;

	m_velocity.y = Math::clampedAdd(-3000.f, 3000.f, m_velocity.y, 1000.f * dt);

	if (!grounded)
	{
		accel = 3000.f;
		friction = .99f;
	}

	if (input.moveDirection > 0)
		m_velocity.x = Math::clampedAdd(-maxSpeed, maxSpeed, m_velocity.x, accel * dt);
	else if (input.moveDirection < 0)
		m_velocity.x = Math::clampedAdd(-maxSpeed, maxSpeed, m_velocity.x, -accel * dt);
	else
		m_velocity.x = m_velocity.x * friction;



	if (grounded)
	{
		if (input.jump)
		{
			m_velocity.y = -700.f;
		}
	}




	sf::Vector2f d = m_velocity * dt;


	sf::Vector2f out;
	sf::Vector2i norm;
	sf::Vector2i tile;
	if (map.sweepCharacter(aabb, d, out, norm, tile))
	{
		if (norm.x)
			m_velocity.x = 0.f;
		if (norm.y)
			m_velocity.y = 0.f;
		d = out;
	}


	m_position += d;
}


void ZombieCore::read(const NetZombie & nz)
{
	m_position.x = nz.pos.x / 100.f;
	m_position.y = nz.pos.y / 100.f;
	m_velocity.x = nz.vel.x / 100.f;
	m_velocity.y = nz.vel.y / 100.f;
}

void ZombieCore::write(NetZombie & nz) const
{
	nz.vel.x = Math::roundToInt(m_velocity.x * 100.f);
	nz.vel.y = Math::roundToInt(m_velocity.y * 100.f);
	nz.pos.x = Math::roundToInt(m_position.x * 100.f);
	nz.pos.y = Math::roundToInt(m_position.y * 100.f);
}

const sf::Vector2f & ZombieCore::getPosition() const
{
	return m_position;
}
