#include "Projectile.h"
#include "Game/NetObject.h"
#include "Game/Snapshot.h"
#include "Core/Utility.h"
#include "Core/Client/PlayingScreen.h"
#include "Core/Client/Particles.h"
#include <SFML/Graphics.hpp>

Projectile::Projectile(int id, Client & client, PlayingScreen & screen):
	Entity(id, EntityType::PROJECTILE, client, screen)
{
	m_emitter = new ParticleEmitter;
	m_emitter->setEmissionRate(200.f);
	m_emitter->setParticleLifeTime(1.f);
	m_emitter->setParticleVelocity(Math::circle(sf::Vector2f(), 25.f));
	m_emitter->setParticlePosition(sf::Vector2f());
	m_emitter->setParticleScale(Math::uniform(.05f, .1f));
	m_emitter->setParticleColor(sf::Color(231, 231, 231, 10));
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
	if (p1)
	{
		pos = Math::lerp(static_cast<sf::Vector2f>(p0->pos) / 100.f, static_cast<sf::Vector2f>(p1->pos) / 100.f, t);
	}

	sf::CircleShape c;
	c.setFillColor(sf::Color::Black);
	c.setPosition(pos);
	c.setRadius(12.5f);
	target.draw(c);

	m_emitter->setParticlePosition(pos + sf::Vector2f(25.f, 25.f) / 2.f);
	
}
