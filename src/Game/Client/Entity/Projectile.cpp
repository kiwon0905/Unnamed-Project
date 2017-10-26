#include "Projectile.h"
#include "Game/NetObject.h"
#include "Game/Snapshot.h"
#include "Game/Client/PlayingScreen.h"
#include "Core/Utility.h"
#include "Core/Client/Particles.h"
#include <SFML/Graphics.hpp>

Projectile::Projectile(int id, Client & client, PlayingScreen & screen):
	Entity(id, NetObject::PROJECTILE, client, screen)
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


	sf::RectangleShape r;
	r.setFillColor(sf::Color::Black);
	r.setSize({ 25.f, 25.f });
	r.setOrigin(r.getSize() / 2.f);
	r.setPosition(pos);
	float angle = atan2f(vel.y, vel.x);
	r.setRotation(angle * 180 / Math::PI);
	target.draw(r);


	sf::Vector2f a = Math::rotatePoint({ 12.5f, 12.5f }, angle);
	sf::Vector2f b = Math::rotatePoint({ 12.5f, 0.f }, angle);
	sf::Vector2f c = Math::rotatePoint({ 12.5f, -12.5f }, angle);

	sf::RectangleShape ar{ {5.f, 5.f} };
	ar.setFillColor(sf::Color::White);
	ar.setOrigin(2.5f, 2.5f);
	ar.setPosition(a + pos);

	sf::RectangleShape br{ { 5.f, 5.f } };
	br.setFillColor(sf::Color::White);
	br.setOrigin(2.5f, 2.5f);
	br.setPosition(b + pos);

	sf::RectangleShape cr{ { 5.f, 5.f } };
	cr.setFillColor(sf::Color::White);
	cr.setOrigin(2.5f, 2.5f);
	cr.setPosition(c + pos);
	
	target.draw(ar);
	target.draw(br);

	target.draw(cr);




	sf::Vector2f smokeOffset;
	smokeOffset -= Math::unit(vel) * 25.f;

	m_emitter->pos = pos + smokeOffset;
	
}
