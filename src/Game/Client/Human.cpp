#include "Human.h"
#include "Game/Snapshot.h"
#include "Core/Utility.h"
#include "Core/Client/PlayingScreen.h"

#include <SFML/Graphics.hpp>

Human::Human(int id, Client & client, PlayingScreen & screen):
	Entity(id, EntityType::HUMAN, client, screen)
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

void Human::render(const Snapshot * from, const Snapshot * to, float predictedT, float t)
{
	sf::RenderWindow & target = m_client->getWindow();
	const NetHuman * h0 = static_cast<const NetHuman*>(from->getEntity(m_id));
	const NetHuman * h1 = nullptr;
	if (to)
		h1 = static_cast<const NetHuman*>(to->getEntity(m_id));

	sf::Vector2f pos = getRenderPos(h0, h1, predictedT, t);

	//body
	sf::RectangleShape body;
	body.setSize({ 69.f, 69.f });
	sf::Color teamColor = m_screen->getPlayerInfo(m_id)->team == Team::A ? sf::Color::Blue : sf::Color::Red;
	body.setFillColor(teamColor);
	body.setOutlineColor(sf::Color::Black);
	body.setOutlineThickness(-3.f);
	body.setPosition(pos);
	target.draw(body);

	//gun
	sf::RectangleShape gun;
	gun.setSize({ 60.f, 20.f });
	gun.setOrigin({ 0.f / 5.f, gun.getSize().y / 2.f });
	gun.setPosition(pos + sf::Vector2f(69.f, 69.f) / 2.f);
	gun.setOutlineColor(sf::Color::Black);
	gun.setOutlineThickness(-3.f);

	float aimAngle;
	if (m_predicted)
	{
		sf::Vector2i mousePos = m_client->getInput().getMousePosition();
		sf::Vector2f aim = target.mapPixelToCoords(mousePos) - (body.getPosition() + body.getSize() / 2.f);
		aimAngle = atan2f(aim.y, aim.x) * 180.f / Math::PI;
	}
	else
	{
		aimAngle = static_cast<float>(h0->aimAngle);
		if (h1)
			aimAngle = Math::lerp(static_cast<float>(h0->aimAngle), static_cast<float>(h1->aimAngle), t);
	}

	gun.setRotation(aimAngle);
	target.draw(gun);

	//health

	sf::RectangleShape total;
	total.setSize({ 70.f, 15.f });
	total.setFillColor(sf::Color(112, 128, 144));
	total.setOutlineColor(sf::Color::Black);
	total.setOutlineThickness(-2.f);

	total.setPosition({ pos.x + 69.f / 2.f - 35.f, pos.y - 45.f });
	target.draw(total);

	sf::RectangleShape health;
	float healthVal = h0->health;
	if (h1)
		healthVal = Math::lerp<float>(h0->health, h1->health, t);
	health.setFillColor(sf::Color::Red);
	if (m_screen->getMyPlayerInfo().entityId == m_id)
		health.setFillColor(sf::Color::Green);
	health.setSize(sf::Vector2f(healthVal / 100.f * (70.f + 2 * total.getOutlineThickness()), total.getSize().y + total.getOutlineThickness() * 2));
	health.setPosition(total.getPosition() - sf::Vector2f{ total.getOutlineThickness(), total.getOutlineThickness() });

	target.draw(health);
}

sf::Vector2f Human::getRenderPos(const NetHuman * h0, const NetHuman * h1, float predictedT, float t) const
{
	if (m_predicted)
		return Math::lerp(m_prevCore.getPosition(), m_currentCore.getPosition(), predictedT);
	else
	{
		sf::Vector2f pos = static_cast<sf::Vector2f>(h0->pos) / 100.f;

		if (h1)
		{
			pos.x = Math::lerp(h0->pos.x / 100.f, h1->pos.x / 100.f, t);
			pos.y = Math::lerp(h0->pos.y / 100.f, h1->pos.y / 100.f, t);
		}
		return pos;
	}
}

