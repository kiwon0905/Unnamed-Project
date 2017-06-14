
#include "Projectile.h"
#include "GameWorld.h"
#include "Game/NetObject.h"
#include "Game/Map.h"

Projectile::Projectile(int id, int shooterId):
	Entity(id, EntityType::PROJECTILE, nullptr),
	m_shooterId(shooterId)
{
	m_size = { 25.f, 25.f };
}

void Projectile::tick(float dt, GameWorld & gameWorld)
{
	Aabb aabb = getAabb();

	sf::Vector2f d = m_velocity * dt;
	

	float time;
	sf::Vector2i norm;
	int tile = gameWorld.getMap().sweep(aabb, d, time, norm);
	if (tile)
	{
		m_alive = false;
		d *= time;
		aabb.x += d.x;
		aabb.y += d.y;
		m_position = { aabb.x, aabb.y };
		d = { 0.f, 0.f };
	}
	


	for (auto & e : gameWorld.getEntities(EntityType::HUMAN))
	{
		if (e->getId() != m_shooterId && aabb.sweep(d, e->getAabb(), time, norm))
		{
			m_alive = false;
			d *= time;
			aabb.x += d.x;
			aabb.y += d.y;
			m_position = { aabb.x, aabb.y };
			d = { 0.f, 0.f };
		}
	}
	m_position += d;
}



void Projectile::snap(Snapshot & snapshot) const
{
	NetProjectile * np = static_cast<NetProjectile*>(snapshot.addEntity(NetObject::PROJECTILE, m_id));
	if (np)
	{
		np->pos.x = (int)std::round(m_position.x * 100.f);
		np->pos.y = (int)std::round(m_position.y * 100.f);
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
