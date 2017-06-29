#include "Zombie.h"
#include "Core/Utility.h"
#include "Game/Snapshot.h"

Zombie::Zombie(int id):
	Entity(id, EntityType::ZOMBIE)
{
}

void Zombie::rollback(const NetObject & e)
{
	const NetZombie & nz = static_cast<const NetZombie&>(e);
	m_prevCore.assign(&nz);
	m_currentCore.assign(&nz);
}

void Zombie::tick(float dt, const NetInput & input, Map & map)
{
	if (m_predicted)
	{
		m_prevCore = m_currentCore;
		m_currentCore.tick(dt, input, map);
	}
}

void Zombie::preRender(const Snapshot * from, const Snapshot * to, float t)
{
	if (m_predicted)
	{
		m_position = lerp(m_prevCore.getPosition(), m_currentCore.getPosition(), t);
	}
	else
	{
		const NetZombie * z0 = static_cast<const NetZombie*>(from->getEntity(m_id));
		const NetZombie * z1 = nullptr;

		if (to)
			z1 = static_cast<const NetZombie*>(to->getEntity(m_id));


		m_position = static_cast<sf::Vector2f>(z0->pos) / 100.f;
		if (z1)
		{
			m_position.x = lerp(z0->pos.x / 100.f, z1->pos.x / 100.f, t);
			m_position.y = lerp(z0->pos.y / 100.f, z1->pos.y / 100.f, t);
		}
	}
}

void Zombie::render(sf::RenderTarget & target, Client & client)
{
	sf::RectangleShape body;
	body.setSize({ 70.f, 70.f });
	body.setFillColor(sf::Color::Blue);
	body.setOutlineColor(sf::Color::Black);
	body.setOutlineThickness(-3.f);
	body.setPosition(m_position);
	target.draw(body);
}
