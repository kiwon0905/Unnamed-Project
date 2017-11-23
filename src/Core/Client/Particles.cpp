#include "Particles.h"

void Particle::update(float dt)
{
	float t = dt / (lifeTime - time);
	scale = Math::lerp(scale, endScale, t);
	vel = Math::lerp(vel, endVel, t);

	time += dt;

	pos += vel * dt;
}

ParticleEmitter::ParticleEmitter(float emissionRate):
	m_emissionRate(emissionRate)
{
}

void ParticleEmitter::update(float dt, Particles & particles)
{
	m_accumulator += dt * m_emissionRate;

	std::size_t count = static_cast<std::size_t>(m_accumulator);
	for (std::size_t i = 0; i < count; ++i)
	{
		Particle & p = particles.emit();
		onEmit(p);

	}
	m_accumulator -= count;
}

Particles::Particles()
{
	m_vertices.setPrimitiveType(sf::PrimitiveType::Quads);

	m_textureRects[ParticleType::EXPLOSION] = sf::IntRect(0, 0, 390, 400);
	m_textureRects[ParticleType::CIRCLE] = sf::IntRect(463, 34, 169, 155);

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

		sf::IntRect rect = m_textureRects[p.type];
		sf::Vector2f pos = sf::Vector2f(static_cast<float>(rect.left), static_cast<float>(rect.top));
		sf::Vector2f size = sf::Vector2f(static_cast<float>(rect.width), static_cast<float>(rect.height));
		//a b 
		//c d 
		sf::Vertex a, b, c, d;
		a.texCoords = sf::Vector2f(pos.x, pos.y);
		b.texCoords = sf::Vector2f(pos.x + size.x, pos.y);
		c.texCoords = sf::Vector2f(pos.x + size.x, pos.y + size.y);
		d.texCoords = sf::Vector2f(pos.x, pos.y + size.y);
	
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