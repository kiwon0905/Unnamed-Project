
#include "Projectile.h"
#include "GameWorld.h"
#include "GameContext.h"
#include "Game/NetObject.h"
#include "Game/Map.h"
#include "Core/Server/Peer.h"

Projectile::Projectile(int id, GameContext * context, int shooterId, Team shooterTeam):
	Entity(id, EntityType::PROJECTILE, context),
	m_shooterId(shooterId),
	m_team(shooterTeam)
{
	m_size = { 25.f, 25.f };
}

void Projectile::tick(float dt)
{
	Aabb aabb = getAabb();

	sf::Vector2f d = m_velocity * dt;
	
	float minTime = 1.f;
	sf::Vector2i minNorm;

	//tile
	m_context->getMap().sweep(aabb, d, minTime, minNorm);


	Human * hitEntity = nullptr;

	for (auto & e : m_context->getWorld().getEntities(EntityType::HUMAN))
	{
		if (e->getId() == m_shooterId)
			continue;
		Human * h = static_cast<Human*>(e.get());

		if (m_context->getPeer(h->getPeerId())->getTeam() != m_team)
		{
			float time;
			sf::Vector2i norm;

			if (aabb.sweep(d * minTime, h->getAabb(), time, norm))
			{
				if (time < minTime)
				{
					minTime = time;
					minNorm = norm;
					hitEntity = h;
				}
			}
		}
	}

	if (minTime != 1.f)
	{
		m_alive = false;
		if (hitEntity)
			hitEntity->takeDamage(10);
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
