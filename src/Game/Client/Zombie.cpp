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
	m_prevCore.read(nz);
	m_currentCore.read(nz);
}

void Zombie::tick(float dt, const NetInput & input, Map & map)
{
	if (m_predicted)
	{
		m_prevCore = m_currentCore;
		m_currentCore.tick(dt, input, map);
	}
}

sf::Vector2f Zombie::getCameraPosition(const Snapshot * from, const Snapshot * to, float predictedT, float t) const
{
	const NetZombie * z0 = static_cast<const NetZombie*>(from->getEntity(m_id));
	const NetZombie * z1 = nullptr;
	if (to)
		z1 = static_cast<const NetZombie*>(to->getEntity(m_id));

	return getRenderPos(z0, z1, predictedT, t) + sf::Vector2f{ 69.f, 69.f };
}

void Zombie::render(sf::RenderTarget & target, Client & client, PlayingScreen & ps, const Snapshot * from, const Snapshot * to, float predictedT, float t)
{
	const NetZombie * z0 = static_cast<const NetZombie*>(from->getEntity(m_id));
	const NetZombie * z1 = nullptr;
	if (to)
		z1 = static_cast<const NetZombie*>(to->getEntity(m_id));

	sf::Vector2f pos = getRenderPos(z0, z1, predictedT, t);
	sf::RectangleShape body;
	body.setSize({ 70.f, 70.f });
	body.setFillColor(sf::Color::Blue);
	body.setOutlineColor(sf::Color::Black);
	body.setOutlineThickness(-3.f);
	body.setPosition(pos);
	target.draw(body);
}

sf::Vector2f Zombie::getRenderPos(const NetZombie * z0, const NetZombie * z1, float predictedT, float t) const
{
	if (m_predicted)
		return lerp(m_prevCore.getPosition(), m_currentCore.getPosition(), predictedT);
	else
	{
		sf::Vector2f pos = static_cast<sf::Vector2f>(z0->pos) / 100.f;

		if (z1)
		{
			pos.x = lerp(z0->pos.x / 100.f, z1->pos.x / 100.f, t);
			pos.y = lerp(z0->pos.y / 100.f, z1->pos.y / 100.f, t);
		}
		return pos;
	}
}
