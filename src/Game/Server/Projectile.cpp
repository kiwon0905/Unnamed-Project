
#include "Projectile.h"
#include "GameWorld.h"
#include "Game/NetObject.h"
#include "Game/Map.h"

Projectile::Projectile(int id):
	Entity(id, EntityType::PROJECTILE, nullptr)
{
}

void Projectile::tick(float dt, GameWorld & gameWorld)
{
	Aabb<float> aabb(m_position.x, m_position.y, 25.f, 25.f);
	MoveResult result = gameWorld.getMap().move(aabb, m_velocity * dt);
	
	if (result.horizontalTile || result.verticalTile)
		m_alive = false;

	m_position += result.v;
}



void Projectile::snap(Snapshot & snapshot) const
{
	NetProjectile * np = static_cast<NetProjectile*>(snapshot.addEntity(NetObject::PROJECTILE, m_id));
	if (np)
	{
		np->position = m_position;
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
