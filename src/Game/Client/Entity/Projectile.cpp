#include "Projectile.h"
#include "Game/NetObject/NetProjectile.h"
#include "Game/Snapshot.h"
#include "Game/Client/PlayingScreen.h"
#include "Core/Utility.h"
#include "Core/Client/ParticleEffects.h"
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

const void * Projectile::find(const Snapshot & s)
{
	return s.getEntity(NetObject::PROJECTILE, m_id);
}

sf::Vector2f Projectile::getCameraPosition() const
{
	return sf::Vector2f();
}

void Projectile::render()
{
	sf::RenderWindow & target = m_client->getWindow();

	const Snapshot * currentSnap = m_screen->getCurrentSnap().snapshot;
	const Snapshot * nextSnap = m_screen->getNextSnap().snapshot;
	const NetProjectile * p0 = static_cast<const NetProjectile*>(currentSnap->getEntity(NetObject::PROJECTILE, m_id));
	const NetProjectile * p1 = nextSnap ? static_cast<const NetProjectile*>(nextSnap->getEntity(NetObject::PROJECTILE, m_id)) : nullptr;


	sf::Vector2f pos = static_cast<sf::Vector2f>(p0->pos) / 100.f;
	sf::Vector2f vel = static_cast<sf::Vector2f>(p0->vel) / 100.f;
	if (p1)
	{
		pos = Math::lerp(static_cast<sf::Vector2f>(p0->pos) / 100.f, static_cast<sf::Vector2f>(p1->pos) / 100.f, m_screen->getRenderInterTick());
		vel = Math::lerp(static_cast<sf::Vector2f>(p0->vel) / 100.f, static_cast<sf::Vector2f>(p1->vel) / 100.f, m_screen->getRenderInterTick());
	}


	sf::RectangleShape r;
	r.setFillColor(sf::Color::Black);
	if (p0->type == ProjectileType::BULLET)
		r.setSize({ 10.f, 10.f });
	else if (p0->type == ProjectileType::ROCKET)
		r.setSize({ 50.f, 25.f });
	r.setOrigin(r.getSize() / 2.f);
	r.setPosition(pos);
	float angle = atan2f(vel.y, vel.x);
	r.setRotation(angle * 180 / Math::PI);
	target.draw(r);


	sf::Vector2f smokeOffset;
	smokeOffset -= Math::unit(vel) * 25.f;

	m_emitter->pos = pos + smokeOffset;
	
}
