
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
	


	float minTime = 1.f;
	sf::Vector2i minNorm;

	
	gameWorld.getMap().sweep(aabb, d, minTime, minNorm);


	Entity * hitEntity = nullptr;
	for (auto & e : gameWorld.getEntities(EntityType::HUMAN))
	{
		float time;
		sf::Vector2i norm;
		if (e->getId() != m_shooterId && aabb.sweep(d, e->getAabb(), time, norm))
		{
			if (time < minTime)
			{
				minTime = time;
				minNorm = norm;
				hitEntity = e.get();
			}
		}
	}

	if (minTime != 1.f)
	{
		m_alive = false;
		if (hitEntity && hitEntity->getId() != m_shooterId)
		{
			Human * h = static_cast<Human*>(hitEntity);
			h->takeDamage(10);
		}
	}

	d *= minTime;

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
