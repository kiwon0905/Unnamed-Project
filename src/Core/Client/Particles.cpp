#include "Particles.h"

void Particle::update(float dt)
{
	float t = dt / (lifeTime - time);
	time += dt;

	pos += vel * dt;
}

void ParticleEmitter::setParticleVelocity(Math::DistributionFunc<sf::Vector2f> fn)
{
	m_velocity = fn;
}

void ParticleEmitter::setParticlePosition(Math::DistributionFunc<sf::Vector2f> fn)
{
	m_position = fn;
}

void ParticleEmitter::setParticleScale(Math::DistributionFunc<float> fn)
{
	m_scale = fn;
}

void ParticleEmitter::setParticleLifeTime(Math::DistributionFunc<float> fn)
{
	m_lifeTime = fn;
}

void ParticleEmitter::setParticleColor(Math::DistributionFunc<sf::Color> color)
{
	m_color = color;
}

void ParticleEmitter::setParticleType(Math::DistributionFunc<ParticleType> type)
{
	m_type = type;
}

void ParticleEmitter::setEmissionRate(float t)
{
	m_emissionRate = t;
}

void ParticleEmitter::update(float dt, Particles & particles)
{
	m_accumulator += dt * m_emissionRate;

	std::size_t count = static_cast<std::size_t>(m_accumulator);
	for (std::size_t i = 0; i < count; ++i)
	{
		Particle & p = particles.emit();
		p.pos = m_position();
		p.vel = m_velocity();
		p.scale = m_scale();
		p.lifeTime = m_lifeTime();
		p.color = m_color();
		p.type = m_type();
	}
	m_accumulator -= count;

}

Particles::Particles()
{
	m_vertices.setPrimitiveType(sf::PrimitiveType::Quads);
}

void Particles::setTexture(const sf::Texture & texture)
{
	m_texture = &texture;
}

void Particles::addEmitter(ParticleEmitter * emitter)
{
	m_emitters.emplace_back(emitter);
}

void Particles::removeEmitter(ParticleEmitter * emitter)
{
	auto pred = [emitter](const std::unique_ptr<ParticleEmitter> & e)
	{
		return emitter == e.get();
	};
	m_emitters.erase(std::remove_if(m_emitters.begin(), m_emitters.end(), pred), m_emitters.end());
}

void Particles::update(float dt)
{
	for (auto & emitter : m_emitters)
		emitter->update(dt, *this);


	for (auto & p : m_particles)
		p.update(dt);

	auto dead = [](const Particle & p)
	{
		return p.time >= p.lifeTime;
	};
	m_particles.erase(std::remove_if(m_particles.begin(), m_particles.end(), dead), m_particles.end());


	//compute vertices
	m_vertices.clear();

	for (Particle & p : m_particles)
	{
		sf::Transform t;
		t.translate(p.pos);
		t.scale({ p.scale, p.scale });

		sf::Vector2f size = static_cast<sf::Vector2f>(m_texture->getSize());
		//a b 
		//c d 
		sf::Vertex a, b, c, d;
		a.texCoords = sf::Vector2f(0.f, 0.f);
		b.texCoords = sf::Vector2f(size.x, 0.f);
		c.texCoords = sf::Vector2f(size.x, size.y);
		d.texCoords = sf::Vector2f(0.f, size.y);
	
		a.position = t.transformPoint(-size / 2.f);
		b.position = t.transformPoint(sf::Vector2f(size.x, -size.y) / 2.f);
		c.position = t.transformPoint(size / 2.f);
		d.position = t.transformPoint(sf::Vector2f(-size.x, size.y) / 2.f);

		
		a.color = p.color;
		b.color = p.color;
		c.color = p.color;
		d.color = p.color;

		m_vertices.append(a);
		m_vertices.append(b);
		m_vertices.append(c);
		m_vertices.append(d);

	}
}

void Particles::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	states.texture = m_texture;
	target.draw(m_vertices, states);
}

Particle & Particles::emit()
{
	m_particles.emplace_back();
	return m_particles.back();
}
