
#include "Projectile.h"
#include "Human.h"
#include "TransientEntities.h"
#include "Game/Server/GameWorld.h"
#include "Game/Server/GameContext.h"
#include "Game/NetObject/NetProjectile.h"
#include "Game/Map.h"
#include "Core/Server/Peer.h"
#include "Core/Server/Server.h"
#include "Core/Utility.h"

Projectile::Projectile(int id, GameContext * context, ProjectileType type, int shooterPeerId, Team shooterTeam):
	Entity(id, EntityType::PROJECTILE, context),
	m_type(type),
	m_shooterPeerId(shooterPeerId),
	m_team(shooterTeam)
{
	if (type == ProjectileType::BULLET)
	{
		m_size = { 10.f, 10.f };
	}
	else if (type == ProjectileType::ROCKET)
	{
		m_size = { 50.f, 25.f };
	}
}

void Projectile::tick(float dt)
{
	m_velocity.y = Math::clampedAdd(-3000.f, 3000.f, m_velocity.y, m_gravity * dt);
	
	
	float angle = atan2f(m_velocity.y, m_velocity.x);

	//collision points
	sf::Vector2f a = m_position + Math::rotatePoint({ m_size.x / 2.f, m_size.y / 2.f }, angle);
	sf::Vector2f b = m_position + Math::rotatePoint({ m_size.x / 2.f, 0.f }, angle);
	sf::Vector2f c = m_position + Math::rotatePoint({ m_size.x / 2.f, -m_size.y / 2.f }, angle);


	sf::Vector2f d = m_velocity * dt;
	
	float minTime = 1.f;
	bool collided = false;
	Character * hitEntity = nullptr;

	//tile
	if (m_context->getMap().sweepPoints({ a, b, c }, d, minTime))
	{
		collided = true;
	}

	for (auto e : m_context->getWorld().getEntities(EntityType::CHARACTER))
	{
		//no self dmg
		if (m_context->getPlayer(m_shooterPeerId) && m_context->getPlayer(m_shooterPeerId)->getCharacter() &&
			m_context->getPlayer(m_shooterPeerId)->getCharacter()->getId() == e->getId())
			continue;

		if (m_context->getPlayer(static_cast<Human*>(e)->getPeerId())->getTeam() == m_team)
			continue;

		Aabb aabb{ e->getPosition(), e->getSize() };
		float t;

		if (aabb.sweepPoints({ a, b, c }, d, t))
		{
			collided = true;
			if (t < minTime)
			{
				minTime = t;
				hitEntity = static_cast<Character*>(e);
			}
		}
		
	}

	
	if (collided)
	{
		m_alive = false;
		if (hitEntity)
		{
			sf::Vector2f entCenter = hitEntity->getPosition() + hitEntity->getSize() / 2.f;
			hitEntity->takeDamage(10, m_shooterPeerId, Math::unit(entCenter - m_position) * 500.f);
		}

		m_context->getWorld().createTransientEntity<Explosion>(m_position + m_size / 2.f);
	}

	m_position += d;
}



void Projectile::snap(Snapshot & snapshot) const
{
	NetProjectile * np = static_cast<NetProjectile*>(snapshot.addEntity(NetObject::PROJECTILE, m_id));
	if (np)
	{
		np->type = m_type;
		np->pos.x = static_cast<int>(std::round(m_position.x * 100.f));
		np->pos.y = static_cast<int>(std::round(m_position.y * 100.f));
		np->vel.x = static_cast<int>(std::round(m_velocity.x * 100.f));
		np->vel.y = static_cast<int>(std::round(m_velocity.y * 100.f));
	}
}

void Projectile::setPosition(sf::Vector2f v)
{
	m_position = v;
}

void Projectile::setVelocity(sf::Vector2f v)
{
	m_velocity = v;
}
