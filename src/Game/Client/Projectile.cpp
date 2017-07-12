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

sf::Vector2f Projectile::getCameraPosition(const Snapshot * from, const Snapshot * to, float predictedT, float t) const
{
	return sf::Vector2f();
}

void Projectile::render(sf::RenderTarget & target, Client & client, PlayingScreen & ps, const Snapshot * from, const Snapshot * to, float predictedT, float t)
{
	const NetProjectile * p0 = static_cast<const NetProjectile*>(from->getEntity(m_id));
	const NetProjectile * p1 = nullptr;

	if (to)
		p1 = static_cast<const NetProjectile*>(to->getEntity(m_id));


	sf::Vector2f pos = static_cast<sf::Vector2f>(p0->pos) / 100.f;
	if (p1)
	{
		pos = lerp(static_cast<sf::Vector2f>(p0->pos) / 100.f, static_cast<sf::Vector2f>(p1->pos) / 100.f, t);
	}

	sf::CircleShape c;
	c.setFillColor(sf::Color::Black);
	c.setPosition(pos);
	c.setRadius(12.5f);
	target.draw(c);
}
