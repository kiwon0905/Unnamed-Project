#include "Projectile.h"
#include "Game/NetObject.h"
#include "Core/Utility.h"
#include <SFML/Graphics.hpp>

Projectile::Projectile(int id):
	Entity(id, EntityType::PROJECTILE)
{

}

void Projectile::renderPast(const NetObject * from, const NetObject * to, float t, sf::RenderTarget & target)
{
	const NetProjectile * p0 = static_cast<const NetProjectile*>(from);
	const NetProjectile * p1 = static_cast<const NetProjectile*>(to);
	sf::Vector2f pos = p0->position;

	if (to)
		pos = lerp(p0->position, p1->position, t);

	sf::RectangleShape r;
	r.setFillColor(sf::Color::Black);
	r.setPosition(pos);
	r.setSize({ 25.f, 25.f });
	target.draw(r);
}

void Projectile::renderFuture(const CharacterCore & prevCore, const CharacterCore & prevCurrent, float t, sf::RenderTarget & target)
{

}