#include "Human.h"
#include "Projectile.h"
#include "Game/GameConfig.h"
#include "Game/Netinput.h"
#include "Game/NetObject.h"
#include "Game/Server/GameContext.h"
#include "Game/Server/GameWorld.h"
#include "Core/Server/Peer.h"
#include "Core/Server/Server.h"
#include "Core/Utility.h"
#include "Game/GameConfig.h"

Human::Human(int id, GameContext * context, int peerId, const sf::Vector2f & pos) :
	Character(id, context, peerId, pos)
{
	m_size = { 69.f, 69.f };
	m_core.setPosition(pos);
}

void Human::tick(float dt)
{
	Character::tick(dt);

	NetInput input = getInput();

	m_core.tick(dt, input, m_context->getMap());
	m_position = m_core.getPosition();

	sf::Vector2f center = m_position + m_size / 2.f;
	sf::Vector2f v = input.aimDirection - center;
	if (v == sf::Vector2f())
		v = sf::Vector2f(0.f, -1.f);
	m_aimAngle = atan2f(v.y, v.x) * 180.f / Math::PI;
	
	if (input.fire)
	{
		if (m_fireCooldown == 0)
		{
			sf::Vector2f firePos = center + Math::unit(v) * 60.f;// -sf::Vector2f(25.f, 25.f) / 2.f;
			if (m_context->getMap().getTile(firePos.x, firePos.y) == 0 &&
				m_context->getMap().getTile(firePos.x + 25.f, firePos.y) == 0 &&
				m_context->getMap().getTile(firePos.x + 25.f, firePos.y + 25.f) == 0 &&
				m_context->getMap().getTile(firePos.x, firePos.y + 25.f) == 0)
			{
				Projectile * p = m_context->getWorld().createEntity<Projectile>(ProjectileType::ROCKET, m_peerId, getPlayer()->getTeam());

				p->setVelocity(Math::unit(v) * 1500.f);
				p->setPosition(firePos);
				m_fireCooldown = 10;
			}
		}
	}

	m_fireCooldown--;
	if (m_fireCooldown < 0)
		m_fireCooldown = 0;
	Aabb aabb{ m_position, m_size };

	if (m_context->getMap().leftMap(aabb))
	{
		die(m_peerId);
	}
}

void Human::snap(Snapshot & snapshot) const
{
	NetHuman * h = static_cast<NetHuman*>(snapshot.addEntity(NetObject::HUMAN, m_id));
	if (h)
	{
		m_core.write(*h);
		h->aimAngle = Math::roundToInt(m_aimAngle);
		h->health = m_health;
	}
}

void Human::takeDamage(int dmg, int from, const sf::Vector2f & impulse)
{
	Character::takeDamage(dmg, from, impulse);
	m_core.setVelocity(m_core.getVelocity() + impulse);
}
