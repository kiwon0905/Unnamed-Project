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
	float endScale;

	float time = 0.f;
	float lifeTime;
	
	sf::Color color;	
	ParticleType type;

	void update(float dt);
};

class ParticleEmitter
{
public:
	void setParticlePosition(Math::DistributionFunc<sf::Vector2f> fn);
	
	void setParticleVelocity(Math::DistributionFunc<sf::Vector2f> fn);
	
	void setParticleScale(Math::DistributionFunc<float> fn);
	
	void setParticleLifeTime(Math::DistributionFunc<float> fn);
	
	void setParticleColor(Math::DistributionFunc<sf::Color> color);
	
	void setParticleType(Math::DistributionFunc<ParticleType> type);
	
	
	void setEmissionRate(float t);
	void update(float dt, class Particles & particles);
private:
	Math::DistributionFunc<sf::Vector2f> m_position = sf::Vector2f();

	Math::DistributionFunc<sf::Vector2f> m_velocity = sf::Vector2f();

	Math::DistributionFunc<float> m_scale = 1.f;

	Math::DistributionFunc<float> m_lifeTime = 1.f;

	Math::DistributionFunc<sf::Color> m_color = sf::Color(255, 255, 255, 255);
	Math::DistributionFunc<ParticleType> m_type = ParticleType::CIRCLE;

	float m_emissionRate = 1.f;
	float m_accumulator = 0.f;
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
