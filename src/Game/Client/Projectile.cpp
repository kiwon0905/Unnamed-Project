#include "Projectile.h"
#include "Game/NetObject.h"
#include "Game/Snapshot.h"
#include "Core/Utility.h"
#include <SFML/Graphics.hpp>

Projectile::Projectile(int id):
	Entity(id, EntityType::PROJECTILE)
{

}

void Projectile::rollback(const Snapshot & s)
{

}

void Projectile::tick(float dt, unsigned input, Map & map)
{

}

void Projectile::render(const Snapshot * from, const Snapshot * to, float t, sf::RenderTarget & target)
{
	if (m_predicted)
	{
	}
	else
	{
		const NetProjectile * p0 = static_cast<const NetProjectile*>(from->getEntity(m_id));
		const NetProjectile * p1 = nullptr;
		
		if(to)
			p1 = static_cast<const NetProjectile*>(to->getEntity(m_id));
		sf::Vector2f pos = static_cast<sf::Vector2f>(p0->pos) / 100.f;

		if (p1)
		{
			pos = lerp(static_cast<sf::Vector2f>(p0->pos) / 100.f, static_cast<sf::Vector2f>(p1->pos) / 100.f, t);

		}

		sf::RectangleShape r;
		r.setFillColor(sf::Color::Black);
		r.setPosition(pos);
		r.setSize({ 25.f, 25.f });
		target.draw(r);
	}
}