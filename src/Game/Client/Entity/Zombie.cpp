#include "Zombie.h"
#include "Core/Utility.h"
#include "Game/Client/PlayingScreen.h"
#include "Game/Snapshot.h"

Zombie::Zombie(int id, Client & client, PlayingScreen & screen):
	PredictedEntity(id, NetObject::ZOMBIE, client, screen)
{
}

bool Zombie::rollback(Snapshot & snapshot)
{
	const NetZombie * nz = static_cast<const NetZombie*>(snapshot.getEntity(NetObject::ZOMBIE, m_id));
	if (!nz)
		return false;
	m_prevCore.read(*nz);
	m_currentCore.read(*nz);

	return true;
}

void Zombie::tick(float dt, const NetInput & input, Map & map)
{
	m_prevCore = m_currentCore;
	m_currentCore.tick(dt, input, map);
}

const void * Zombie::find(const Snapshot & s)
{
	return s.getEntity(NetObject::ZOMBIE, m_id);
}

sf::Vector2f Zombie::getCameraPosition(const Snapshot * from, const Snapshot * to, float predictedT, float t) const
{
	const NetZombie * z0 = static_cast<const NetZombie*>(from->getEntity(NetObject::ZOMBIE, m_id));
	const NetZombie * z1 = nullptr;
	if (to)
		z1 = static_cast<const NetZombie*>(to->getEntity(NetObject::ZOMBIE, m_id));

	return getRenderPos(z0, z1, predictedT, t) + sf::Vector2f{ 69.f, 69.f };
}

void Zombie::render(const Snapshot * from, const Snapshot * to, float predictedT, float t)
{
	sf::RenderTarget & target = m_client->getWindow();
	const PlayingScreen::PlayerInfo * info = m_screen->getPlayerInfoByEntity(m_id, NetObject::ZOMBIE);


	const NetZombie * z0 = static_cast<const NetZombie*>(from->getEntity(NetObject::ZOMBIE, m_id));
	const NetZombie * z1 = nullptr;
	if (to)
		z1 = static_cast<const NetZombie*>(to->getEntity(NetObject::ZOMBIE, m_id));

	//body
	sf::Vector2f pos = getRenderPos(z0, z1, predictedT, t);
	sf::RectangleShape body;
	body.setSize({ 70.f, 70.f });
	body.setFillColor(sf::Color::Blue);
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
	if (isPredicted())
	{
		sf::Vector2i mousePos = m_client->getInput().getMousePosition();
		sf::Vector2f aim = target.mapPixelToCoords(mousePos) - (body.getPosition() + body.getSize() / 2.f);
		aimAngle = atan2f(aim.y, aim.x) * 180.f / Math::PI;
	}
	else
	{
		aimAngle = static_cast<float>(z0->aimAngle);
		if (z1)
		{
			int prevAngle = z0->aimAngle;
			int currentAngle = z1->aimAngle;
			int da = currentAngle - prevAngle;
			if (da > 180)
				prevAngle += 360;
			else if (da < -180)
				currentAngle += 360;
			aimAngle = Math::lerp(static_cast<float>(prevAngle), static_cast<float>(currentAngle), t);
		}
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
	float healthVal = static_cast<float>(z0->health);
	if (z1)
		healthVal = Math::lerp<float>(static_cast<float>(z0->health), static_cast<float>(z1->health), t);
	health.setFillColor(sf::Color::Red);


	if (m_screen->getMyPlayerId() == info->id)
		health.setFillColor(sf::Color::Green);
	health.setSize(sf::Vector2f(healthVal / 100.f * (70.f + 2 * total.getOutlineThickness()), total.getSize().y + total.getOutlineThickness() * 2));
	health.setPosition(total.getPosition() - sf::Vector2f{ total.getOutlineThickness(), total.getOutlineThickness() });
	target.draw(health);

	//name
	sf::Text name;
	name.setFont(*m_screen->getFont());
	name.setString(info->name);
	name.setOrigin(name.getLocalBounds().left, name.getLocalBounds().top);
	if (m_screen->getMyPlayerId() == info->id)
		name.setFillColor(sf::Color::Yellow);
	name.setPosition(pos);
	name.setPosition({ pos.x + 69.f / 2.f - name.getLocalBounds().width / 2.f, total.getPosition().y - name.getLocalBounds().height - name.getFont()->getLineSpacing(name.getCharacterSize()) });
	target.draw(name);
}

const ZombieCore & Zombie::getCore() const
{
	return m_currentCore;
}

sf::Vector2f Zombie::getRenderPos(const NetZombie * z0, const NetZombie * z1, float predictedT, float t) const
{
	if (isPredicted())
		return Math::lerp(m_prevCore.getPosition(), m_currentCore.getPosition(), predictedT);
	else
	{
		sf::Vector2f pos = static_cast<sf::Vector2f>(z0->pos) / 100.f;

		if (z1)
		{
			pos.x = Math::lerp(z0->pos.x / 100.f, z1->pos.x / 100.f, t);
			pos.y = Math::lerp(z0->pos.y / 100.f, z1->pos.y / 100.f, t);
		}
		return pos;
	}
}
