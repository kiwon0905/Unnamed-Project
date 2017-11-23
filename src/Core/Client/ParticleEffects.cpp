#include "ParticleEffects.h"

SmokeTrailEmitter::SmokeTrailEmitter() :
	ParticleEmitter(200.f)
{
}

void SmokeTrailEmitter::onEmit(Particle & p)
{
	p.lifeTime = 1.f;
	p.vel = Math::uniformCircle(sf::Vector2f(), 55.f);
	p.endVel = sf::Vector2f();
	p.pos = pos;
	p.scale = Math::uniform(.05f, .1f);
	p.endScale = 0.f;
	p.type = ParticleType::CIRCLE;
}

BulletTrailEmitter::BulletTrailEmitter() :
	ParticleEmitter(200.f)
{
}

void BulletTrailEmitter::onEmit(Particle & p)
{
}


void createExplosion(Particles & p, const sf::Vector2f & pos)
{
	for (int i = 0; i < 24; ++i)
	{
		Particle & smoke = p.emit();
		smoke.pos = pos;
		smoke.vel = Math::uniformCircle(sf::Vector2f(), 1050.f);
		smoke.lifeTime = .5f;
		smoke.scale = .4f;
		smoke.endScale = 0.f;
		smoke.color = sf::Color(232, 232, 232);
		smoke.type = ParticleType::CIRCLE;
	}

	for (int i = 0; i < 1; ++i)
	{
		Particle & core = p.emit();
		core.pos = pos;
		core.lifeTime = .5f;
		core.scale = .4f;
		core.endScale = .0f;
		core.endVel = core.vel = Math::uniformCircle(sf::Vector2f(), 40.f);
		core.type = ParticleType::EXPLOSION;
	}

}