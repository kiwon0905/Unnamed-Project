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

void Human::render(const Snapshot * from, const Snapshot * to, float t, sf::RenderTarget & target)
{
	sf::RectangleShape r;
	r.setSize({ 50.f, 50.f });
	r.setFillColor(sf::Color::Green);
	sf::Vector2f pos;

	if (m_predicted)
	{
		pos = lerp(m_prevCore.getPosition(), m_currentCore.getPosition(), t);
	}
	else
	{
		const NetHuman * h0 = static_cast<const NetHuman*>(from->getEntity(m_id));
		const NetHuman * h1 = nullptr;
		
		if(to)
			h1 = static_cast<const NetHuman*>(to->getEntity(m_id));

		pos = h0->position;

		if (h1)
			pos = lerp(h0->position, h1->position, t);
	}

	r.setPosition(pos);
	target.draw(r);
	m_position = pos;
}

