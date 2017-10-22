#include "Projectile.h"
#include "Game/NetObject.h"
#include "Game/Snapshot.h"
#include "Game/Client/PlayingScreen.h"
#include "Core/Utility.h"
#include "Core/Client/Particles.h"
#include <SFML/Graphics.hpp>

Projectile::Projectile(int id, Client & client, PlayingScreen & screen):
	Entity(id, EntityType::PROJECTILE, client, screen)
{
	m_emitter = new SmokeTrailEmitter;
	m_screen->getParticles().addEmitter(m_emitter);
}

Projectile::~Projectile()
{
	m_screen->getParticles().removeEmitter(m_emitter);
}

sf::Vector2f Projectile::getCameraPosition(const Snapshot * from, const Snapshot * to, float predictedT, float t) const
{
	return sf::Vector2f();
}

void Projectile::render(const Snapshot * from, const Snapshot * to, float predictedT, float t)
{
	sf::RenderWindow & target = m_client->getWindow();
	const NetProjectile * p0 = static_cast<const NetProjectile*>(from->getEntity(m_id));
	const NetProjectile * p1 = nullptr;

	if (to)
		p1 = static_cast<const NetProjectile*>(to->getEntity(m_id));


	sf::Vector2f pos = static_cast<sf::Vector2f>(p0->pos) / 100.f;
	sf::Vector2f vel = static_cast<sf::Vector2f>(p0->vel) / 100.f;
	if (p1)
	{
		pos = Math::lerp(static_cast<sf::Vector2f>(p0->pos) / 100.f, static_cast<sf::Vector2f>(p1->pos) / 100.f, t);
		vel = Math::lerp(static_cast<sf::Vector2f>(p0->vel) / 100.f, static_cast<sf::Vector2f>(p1->vel) / 100.f, t);
	}

	sf::CircleShape c;
	c.setFillColor(sf::Color::Black);
	c.setPosition(pos);
	c.setRadius(12.5f);
	target.draw(c);
	

	sf::Vector2f smokeOffset = sf::Vector2f(25.f, 25.f) / 2.f;
	smokeOffset -= Math::unit(vel) * 25.f;

	m_emitter->pos = pos + smokeOffset;
	
}
