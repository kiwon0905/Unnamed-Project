#include "Human.h"
#include "Game/Snapshot.h"
#include "Game/Client/PlayingScreen.h"
#include "Core/Utility.h"

#include <SFML/Graphics.hpp>

Human::Human(int id, Client & client, PlayingScreen & screen):
	PredictedEntity(id, client, screen)
{
	const sf::Texture * texture = client.getAssetManager().get<sf::Texture>("assets/characters/alienBlue.png");

	m_restAnimation.setSpriteSheet(*texture);
	m_restAnimation.addFrame({ 67, 378, 66, 92 });

	m_walkingAnimation.setSpriteSheet(*texture);
	m_walkingAnimation.addFrame({ 67, 285, 66, 93 });
	m_walkingAnimation.addFrame({ 0, 356, 67, 96 });

	m_inAirAnimation.setSpriteSheet(*texture);
	m_inAirAnimation.addFrame({ 67, 192, 66, 93 });

	m_sprite.setAnimation(m_walkingAnimation);
	m_sprite.setOrigin(m_sprite.getLocalBounds().width / 2.f, m_sprite.getLocalBounds().height / 2.f);
	m_sprite.play();

	//body
	m_body.setSize({ 69.f, 69.f });
	m_body.setOutlineColor(sf::Color::Black);
	m_body.setOutlineThickness(-3.f);

	//gun
	m_gun.setSize({ 60.f, 20.f });
	m_gun.setOutlineColor(sf::Color::Black);
	m_gun.setOutlineThickness(-3.f);

	//health
	m_healthBackground.setSize({ 70.f, 15.f });
	m_healthBackground.setFillColor(sf::Color(112, 128, 144));
	m_healthBackground.setOutlineColor(sf::Color::Black);
	m_healthBackground.setOutlineThickness(-2.f);

	//name
	m_name.setFont(*m_screen->getFont());
}

bool Human::rollback(Snapshot & snapshot)
{
	const NetHuman * nh = static_cast<const NetHuman*>(snapshot.getEntity(NetObject::HUMAN, m_id));
	if (!nh)
		return false;
	m_prevCore.read(*nh);
	m_currentCore.read(*nh);

	return true;
}

const void * Human::find(const Snapshot & s)
{
	return s.getEntity(NetObject::HUMAN, m_id);
}

void Human::tick(float dt, const NetInput & input, Map & map)
{
	m_prevCore = m_currentCore;
	m_currentCore.tick(dt, input, map);
}

sf::Vector2f Human::getCameraPosition() const
{
	const Snapshot * currentSnap = m_screen->getCurrentSnap().snapshot;
	const Snapshot * nextSnap = m_screen->getNextSnap().snapshot;
	const NetHuman * h0 = static_cast<const NetHuman*>(currentSnap->getEntity(NetObject::HUMAN, m_id));
	const NetHuman * h1 = nextSnap ? static_cast<const NetHuman*>(currentSnap->getEntity(NetObject::HUMAN, m_id)) : nullptr;
	return getRenderPos(h0, h1) + sf::Vector2f{ 69.f, 69.f } / 2.f;
}

void Human::render()
{
	const PlayingScreen::PlayerInfo * info = m_screen->getPlayerInfoByEntity(m_id);
	sf::RenderWindow & target = m_client->getWindow();
	sf::Time dt = m_screen->getCurrentRenderTime() - m_screen->getPrevRenderTime();
	if (isPredicted())
		dt = m_screen->getCurrentPredictedTime() - m_screen->getPrevPredictedTime();
	
	const Snapshot * currentSnap = m_screen->getCurrentSnap().snapshot;
	const Snapshot * nextSnap = m_screen->getNextSnap().snapshot;
	const NetHuman * h0 = static_cast<const NetHuman*>(currentSnap->getEntity(NetObject::HUMAN, m_id));
	const NetHuman * h1 = nextSnap ? static_cast<const NetHuman*>(currentSnap->getEntity(NetObject::HUMAN, m_id)) : nullptr;

	sf::Vector2f pos = getRenderPos(h0, h1);
	sf::Vector2f vel = Math::lerp(m_prevCore.getVelocity(), m_currentCore.getVelocity(), m_screen->getPredictedInterTick());
	if (isPredicted())
	{
		vel = Math::lerp(m_prevCore.getVelocity(), m_currentCore.getVelocity(), m_screen->getPredictedInterTick());
	}
	else
	{
		vel = h1 ? Math::lerp(static_cast<sf::Vector2f>(h0->vel) / 100.f, static_cast<sf::Vector2f>(h1->vel), m_screen->getRenderInterTick()) 
			: static_cast<sf::Vector2f>(h0->vel) / 100.f;
	}

	//body
	sf::RectangleShape body;
	body.setSize({ 63.f, 93.f });
	sf::Color teamColor = info->team == Team::A ? sf::Color::Blue : sf::Color::Red;
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
	if (isPredicted())
	{
		sf::Vector2i mousePos = m_client->getInput().getMousePosition();
		sf::Vector2f aim = target.mapPixelToCoords(mousePos) - (body.getPosition() + body.getSize() / 2.f);
		aimAngle = atan2f(aim.y, aim.x) * 180.f / Math::PI;
	}
	else
	{
		aimAngle = static_cast<float>(h0->aimAngle);
		if (h1)
		{
			int prevAngle = h0->aimAngle;
			int currentAngle = h1->aimAngle;
			int da = currentAngle - prevAngle;
			if (da > 180)
				prevAngle += 360;
			else if (da < -180)
				currentAngle += 360;
			aimAngle = Math::lerp(static_cast<float>(prevAngle), static_cast<float>(currentAngle), m_screen->getRenderInterTick());
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
	float healthVal = static_cast<float>(h0->health);
	if (h1)
		healthVal = Math::lerp<float>(static_cast<float>(h0->health), static_cast<float>(h1->health), m_screen->getRenderInterTick());
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
	name.setPosition({ pos.x + 69.f / 2.f - name.getLocalBounds().width / 2.f, total.getPosition().y - name.getLocalBounds().height - name.getFont()->getLineSpacing(name.getCharacterSize())});
	target.draw(name);

	//std::cout << "angle: " << aimAngle << "\n";
	if (aimAngle > 90.f || aimAngle < -90.f)
	{
		m_sprite.setScale({ -1.f, 1.f });
	}
	else
	{
		m_sprite.setScale({ 1.f, 1.f });
	}
	m_sprite.setPosition(pos + m_sprite.getOrigin());

	Aabb aabb{ pos.x, pos.y, 63.f, 93.f };
	bool grounded = m_screen->getMap().isGrounded(aabb);
	std::cout << "grnd: " << grounded << "\n";
	if (grounded)
	{
		float speed = Math::length(vel);
		if (speed < 5.f)
		{
			m_sprite.setAnimation(m_restAnimation);
		}
		else 
		{
			m_sprite.setFrameTime(sf::seconds(1.f / speed));
			if (m_sprite.getAnimation() != &m_walkingAnimation)
			{
				m_sprite.setAnimation(m_walkingAnimation);
				m_sprite.play();
			}

		}
	}
	else
	{
		m_sprite.setAnimation(m_inAirAnimation);
	}
	m_sprite.update(dt);
	target.draw(m_sprite);
}

sf::Vector2f Human::getRenderPos(const NetHuman * h0, const NetHuman * h1) const
{
	if (isPredicted())
		return Math::lerp(m_prevCore.getPosition(), m_currentCore.getPosition(), m_screen->getPredictedInterTick());
	else
	{
		sf::Vector2f pos = static_cast<sf::Vector2f>(h0->pos) / 100.f;

		if (h1)
		{
			pos.x = Math::lerp(h0->pos.x / 100.f, h1->pos.x / 100.f, m_screen->getRenderInterTick());
			pos.y = Math::lerp(h0->pos.y / 100.f, h1->pos.y / 100.f, m_screen->getRenderInterTick());
		}
		return pos;
	}
}

