#include "Projectile.h"
#include "Game/NetObject.h"
#include "Game/Snapshot.h"
#include "Core/Utility.h"
#include <SFML/Graphics.hpp>

Projectile::Projectile(int id):
	Entity(id, EntityType::PROJECTILE)
{

}

void Projectile::rollback(const NetObject & e)
{

}

void Projectile::tick(float dt, const NetInput & input, Map & map)
{

}

void Projectile::preRender(const Snapshot * from, const Snapshot * to, float predictedT, float t)
{
	if (m_predicted)
	{
	}
	else
	{
		const NetProjectile * p0 = static_cast<const NetProjectile*>(from->getEntity(m_id));
		const NetProjectile * p1 = nullptr;

		if (to)
			p1 = static_cast<const NetProjectile*>(to->getEntity(m_id));
		m_position = static_cast<sf::Vector2f>(p0->pos) / 100.f;

		if (p1)
		{
			m_position = lerp(static_cast<sf::Vector2f>(p0->pos) / 100.f, static_cast<sf::Vector2f>(p1->pos) / 100.f, t);
		}
	}
}

void Projectile::render(sf::RenderTarget & target, Client & client)
{
	if (m_predicted)
	{
	}
	else
	{
		sf::CircleShape c;
		c.setFillColor(sf::Color::Black);
		c.setPosition(m_position);
		c.setRadius(12.5f);
		target.draw(c);
	}
}