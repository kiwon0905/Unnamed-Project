#include "Human.h"
#include "Game/Snapshot.h"
#include "Core/Utility.h"
#include "Core/Client/PlayingScreen.h"

#include <SFML/Graphics.hpp>

Human::Human(int id):
	Entity(id, EntityType::HUMAN)
{
}

void Human::rollback(const NetObject & e)
{
	const NetHuman & nh = static_cast<const NetHuman&>(e);

	m_prevCore.read(nh);
	m_currentCore.read(nh);
}

void Human::tick(float dt, const NetInput & input, Map & map)
{
	if (m_predicted)
	{
		m_prevCore = m_currentCore;
		m_currentCore.tick(dt, input, map);
	}
}

void Human::preRender(const Snapshot * from, const Snapshot * to, float predictedT, float t)
{
	if (m_predicted)
	{
		m_position = lerp(m_prevCore.getPosition(), m_currentCore.getPosition(), predictedT);
	}
	else
	{
		const NetHuman * h0 = static_cast<const NetHuman*>(from->getEntity(m_id));
		const NetHuman * h1 = nullptr;

		if (to)
			h1 = static_cast<const NetHuman*>(to->getEntity(m_id));


		m_position = static_cast<sf::Vector2f>(h0->pos) / 100.f;
		m_aimAngle = static_cast<float>(h0->aimAngle);
		if (h1)
		{
			m_position.x = lerp(h0->pos.x / 100.f, h1->pos.x / 100.f, t);
			m_position.y = lerp(h0->pos.y / 100.f, h1->pos.y / 100.f, t);
			m_aimAngle = lerp(static_cast<float>(h0->aimAngle), static_cast<float>(h1->aimAngle), t);
		}
	}
}


void Human::render(sf::RenderTarget & target, Client & client, PlayingScreen & ps)
{
	sf::RectangleShape body;
	body.setSize({ 70.f, 70.f });

	sf::Color teamColor = ps.getPlayerInfo(m_id)->team == Team::A ? sf::Color::Blue : sf::Color::Red;
	body.setFillColor(teamColor);
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
		sf::Vector2f aim = target.mapPixelToCoords(sf::Mouse::getPosition(client.getWindow())) - (body.getPosition() + body.getSize() / 2.f);
		m_aimAngle = atan2f(aim.y, aim.x) * 180.f / PI;
	}

	gun.setRotation(m_aimAngle);
	target.draw(gun);

}

