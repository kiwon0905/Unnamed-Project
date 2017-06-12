
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
	MoveResult result = gameWorld.getMap().move(aabb, m_velocity * dt);
	
	if (result.horizontalTile || result.verticalTile)
	{
		m_alive = false;
	}

	m_position += result.v;

	for (auto & e : gameWorld.getEntities(EntityType::HUMAN))
	{
		if (e->getAabb().intersects(aabb))
		{
			Human & h = static_cast<Human &>(*e);
			//h.takeDamage(10);
			m_alive = false;
		}
	}
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
