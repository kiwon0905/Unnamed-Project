#include "Human.h"
#include "Projectile.h"
#include "GameWorld.h"
#include "Game/GameConfig.h"
#include "Game/Netinput.h"
#include "Game/NetObject.h"
#include "Core/Utility.h"
#include "Core/Server/Peer.h"

Human::Human(int id, Peer * player):
	Entity(id, EntityType::HUMAN, player)
{
	m_size = { 69.f, 69.f };
}

void Human::tick(float dt, GameWorld & world)
{
	NetInput input = m_player->popInput(world.getCurrentTick());

	m_core.tick(dt, input, world.getMap());

	sf::Vector2f center = m_position + m_size / 2.f;
	sf::Vector2f v = input.aimDirection - center;
	if (v == sf::Vector2f())
		v = sf::Vector2f(0.f, -1.f);
	m_aimAngle = atan2f(v.y, v.x) * 180.f / PI;
	m_aimAngle = normalizedAngle(m_aimAngle);
	if (input.fire)
	{
		if (m_fireCooldown == 0)
		{
			sf::Vector2f firePos = center + unit(v) * 60.f - sf::Vector2f(25.f, 25.f) / 2.f;
			//std::cout << "firePos: " <<firePos<<"\n";
			if (world.getMap().getTile(firePos.x, firePos.y) == 0 &&
				world.getMap().getTile(firePos.x + 25.f, firePos.y) == 0 &&
				world.getMap().getTile(firePos.x + 25.f, firePos.y + 25.f) == 0 &&
				world.getMap().getTile(firePos.x, firePos.y + 25.f) == 0)
			{
				Projectile * p = world.createEntity<Projectile>(m_id);

				p->setVelocity(unit(v) * 1000.f);

				p->setPosition(firePos);
				m_fireCooldown = 10;
			}

		}
	}

	m_fireCooldown--;
	if (m_fireCooldown < 0)
		m_fireCooldown = 0;
	m_position = m_core.getPosition();
}

void Human::snap(Snapshot & snapshot) const
{
	NetHuman * h = static_cast<NetHuman*>(snapshot.addEntity(NetObject::HUMAN, m_id));
	if (h)
	{
		h->vel.x = roundToInt(m_core.getVelocity().x * 100.f);
		h->vel.y = roundToInt(m_core.getVelocity().y * 100.f);
		h->pos.x = roundToInt(m_core.getPosition().x * 100.f);
		h->pos.y = roundToInt(m_core.getPosition().y * 100.f);
		h->aimAngle = roundToInt(m_aimAngle);
	}
}

void Human::takeDamage(int dmg)
{
	m_health -= dmg;
	if (m_health <= 0)
		m_alive = false;

	std::cout << "health: " << m_health << "\n";
}
