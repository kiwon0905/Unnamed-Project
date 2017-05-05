#include "Human.h"
#include "Core/Utility.h"
#include "Game/Snapshot.h"
#include <SFML/Graphics.hpp>

Human::Human(int id):
	Entity(id, EntityType::HUMAN)
{
}

void Human::rollback(const Snapshot & s)
{
	const NetHuman * nh = static_cast<const NetHuman*>(s.getEntity(m_id));
	m_prevCore.assign(nh);
	m_currentCore.assign(nh);
}

void Human::tick(float dt, unsigned input, Map & map)
{
	if (m_predicted)
	{
		m_prevCore = m_currentCore;
		m_currentCore.tick(dt, input, map);
	}
}

void Human::preRender(const Snapshot * from, const Snapshot * to, float t)
{
	if (m_predicted)
	{
		m_position = lerp(m_prevCore.getPosition(), m_currentCore.getPosition(), t);
	}
	else
	{
		const NetHuman * h0 = static_cast<const NetHuman*>(from->getEntity(m_id));
		const NetHuman * h1 = nullptr;

		if (to)
			h1 = static_cast<const NetHuman*>(to->getEntity(m_id));


		m_position = static_cast<sf::Vector2f>(h0->pos) / 100.f;
		if (h1)
		{
			m_position.x = lerp(h0->pos.x / 100.f, h1->pos.x / 100.f, t);
			m_position.y = lerp(h0->pos.y / 100.f, h1->pos.y / 100.f, t);
		}
	}
}


void Human::render(sf::RenderTarget & target)
{
	sf::RectangleShape r;
	r.setSize({ 70.f, 70.f });
	r.setFillColor(sf::Color::Green);
	r.setPosition(m_position);
	target.draw(r);
}

