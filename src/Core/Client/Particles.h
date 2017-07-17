#pragma once

#include "Core/Utility.h"
#include <SFML/Graphics.hpp>
#include <memory>


struct Particle
{
	sf::Vector2f pos;
	sf::Vector2f vel;
	float scale;
	float time = 0.f;
	float lifeTime;
	sf::Color color;
};

class ParticleEmitter
{
public:
	void setParticleVelocity(Math::DistributionFunc<sf::Vector2f> fn);
	void setParticlePosition(Math::DistributionFunc<sf::Vector2f> fn);
	void setParticleScale(Math::DistributionFunc<float> fn);
	void setParticleLifeTime(Math::DistributionFunc<float> fn);
	void setParticleColor(Math::DistributionFunc<sf::Color> color);
	void setEmissionRate(float t);
	
	
	void update(float dt, class Particles & particles);
private:
	Math::DistributionFunc<sf::Vector2f> m_velocity = sf::Vector2f();
	Math::DistributionFunc<sf::Vector2f> m_position = sf::Vector2f();
	Math::DistributionFunc<float> m_scale = 1.f;
	Math::DistributionFunc<float> m_lifeTime = 1.f;
	Math::DistributionFunc<sf::Color> m_color = sf::Color(255, 255, 255, 255);
	float m_emissionRate = 1.f;
	float m_accumulator = 0.f;
};


class Particles : public sf::Drawable
{
public:
	Particles(const sf::Texture & texture);

	void addEmitter(ParticleEmitter * emitter);
	void removeEmitter(ParticleEmitter * emitter);

	void update(float dt);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	
	virtual void updateParticle(Particle & p, float dt);

	Particle & emit();
private:
	const sf::Texture * m_texture;
	std::vector<Particle> m_particles;
	std::vector<std::unique_ptr<ParticleEmitter>> m_emitters;

	sf::VertexArray m_vertices;
};

class SmokeParticles : public Particles
{
public:
	SmokeParticles(const sf::Texture & texture);
	void updateParticle(Particle & p, float dt);
};