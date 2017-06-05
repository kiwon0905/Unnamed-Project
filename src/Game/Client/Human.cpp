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

void Human::tick(float dt, const NetInput & input, Map & map)
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


void Human::render(sf::RenderTarget & target, Client & client)
{
	sf::RectangleShape body;
	body.setSize({ 69.f, 69.f });
	body.setFillColor(sf::Color::Green);
	body.setOutlineColor(sf::Color::Black);
	body.setOutlineThickness(-3.f);
	body.setPosition(m_position);
	target.draw(body);

	sf::RectangleShape gun;
	gun.setSize({ 60.f, 20.f });
	gun.setOrigin({ 0.f / 5.f, gun.getSize().y / 2.f });
	gun.setPosition(m_position + sf::Vector2f(69.f, 69.f) / 2.f);
	gun.setOutlineColor(sf::Color::Black);
	gun.setOutlineThickness(-3.f);

	if (m_predicted)
	{

		sf::Vector2f aim = target.mapPixelToCoords(sf::Mouse::getPosition(client.getContext().window)) - (body.getPosition() + body.getSize() / 2.f);
		float angle = atan2f(aim.y, aim.x);
		gun.setRotation(angle * 180.f / PI);
	}
	target.draw(gun);

}

