#pragma once

#include "Core/Utility.h"
#include <SFML/Graphics.hpp>
#include <memory>


enum ParticleType
{
	CIRCLE,
	EXPLOSION,
	COUNT
};

struct Particle
{
	sf::Vector2f pos;
	
	sf::Vector2f vel;
	sf::Vector2f endVel;

	float scale = 1.f;
	float endScale = 1.f;

	float time = 0.f;
	float lifeTime;
	
	sf::Color color;	
	ParticleType type;

	void update(float dt);
};

class ParticleEmitter
{
public:
	ParticleEmitter(float emissionRate);
	virtual ~ParticleEmitter() = default;
	virtual void onEmit(Particle & p) = 0;

	void update(float dt, class Particles & particles);
private:
	float m_emissionRate;
	float m_accumulator = 0.f;
};

class SmokeTrailEmitter : public ParticleEmitter
{
public:
	SmokeTrailEmitter();
	void onEmit(Particle & p);
	sf::Vector2f pos;
};

class Particles : public sf::Drawable
{
public:
	Particles();
	void setTexture(const sf::Texture & t);
	void addEmitter(ParticleEmitter * emitter);
	void removeEmitter(ParticleEmitter * emitter);

	void update(float dt);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	
	Particle & emit();
private:
	const sf::Texture * m_texture;
	std::vector<Particle> m_particles;
	std::vector<std::unique_ptr<ParticleEmitter>> m_emitters;

	sf::VertexArray m_vertices;
};

void createExplosion(Particles & p, const sf::Vector2f & pos);