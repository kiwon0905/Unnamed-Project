#pragma once
#include "Particles.h"

class SmokeTrailEmitter : public ParticleEmitter
{
public:
	SmokeTrailEmitter();
	void onEmit(Particle & p);
	sf::Vector2f pos;
};

class BulletTrailEmitter : public ParticleEmitter
{
public:
	BulletTrailEmitter();
	void onEmit(Particle & p);
	sf::Vector2f pos;
};

void createExplosion(Particles & p, const sf::Vector2f & pos);