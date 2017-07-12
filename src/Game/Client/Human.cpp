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


sf::Vector2f Human::getCameraPosition(const Snapshot * from, const Snapshot * to, float predictedT, float t) const
{
	const NetHuman * h0 = static_cast<const NetHuman*>(from->getEntity(m_id));
	const NetHuman * h1 = nullptr;
	if (to)
		h1 = static_cast<const NetHuman*>(to->getEntity(m_id));
	return getRenderPos(h0, h1, predictedT, t) + sf::Vector2f{ 69.f, 69.f } / 2.f;

}

void Human::render(sf::RenderTarget & target, Client & client, PlayingScreen & ps, const Snapshot * from, const Snapshot * to, float predictedT, float t)
{
	const NetHuman * h0 = static_cast<const NetHuman*>(from->getEntity(m_id));
	const NetHuman * h1 = nullptr;
	if (to)
		h1 = static_cast<const NetHuman*>(to->getEntity(m_id));

	sf::Vector2f pos = getRenderPos(h0, h1, predictedT, t);

	sf::RectangleShape body;
	body.setSize({ 69.f, 69.f });

	sf::Color teamColor = ps.getPlayerInfo(m_id)->team == Team::A ? sf::Color::Blue : sf::Color::Red;
	body.setFillColor(teamColor);
	body.setOutlineColor(sf::Color::Black);
	body.setOutlineThickness(-3.f);
	body.setPosition(pos);
	target.draw(body);

	sf::RectangleShape gun;
	gun.setSize({ 60.f, 20.f });
	gun.setOrigin({ 0.f / 5.f, gun.getSize().y / 2.f });
	gun.setPosition(pos + sf::Vector2f(69.f, 69.f) / 2.f);
	gun.setOutlineColor(sf::Color::Black);
	gun.setOutlineThickness(-3.f);

	float aimAngle;
	if (m_predicted)
	{
		sf::Vector2f aim = target.mapPixelToCoords(sf::Mouse::getPosition(client.getWindow())) - (body.getPosition() + body.getSize() / 2.f);
		aimAngle = atan2f(aim.y, aim.x) * 180.f / PI;
	}
	else
	{
		aimAngle = static_cast<float>(h0->aimAngle);
		if (h1)
			aimAngle = lerp(static_cast<float>(h0->aimAngle), static_cast<float>(h1->aimAngle), t);
	}

	gun.setRotation(aimAngle);
	target.draw(gun);
}

sf::Vector2f Human::getRenderPos(const NetHuman * h0, const NetHuman * h1, float predictedT, float t) const
{
	if (m_predicted)
		return lerp(m_prevCore.getPosition(), m_currentCore.getPosition(), predictedT);
	else
	{
		sf::Vector2f pos = static_cast<sf::Vector2f>(h0->pos) / 100.f;

		if (h1)
		{
			pos.x = lerp(h0->pos.x / 100.f, h1->pos.x / 100.f, t);
			pos.y = lerp(h0->pos.y / 100.f, h1->pos.y / 100.f, t);
		}
		return pos;
	}
}

