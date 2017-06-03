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
	if (input.fire)
	{
		if (m_fireCooldown == 0)
		{
			Projectile * p = world.createEntity<Projectile>(m_id);
			sf::Vector2f v = input.aimDirection - m_core.getPosition();

			p->setPosition(m_core.getPosition());
			p->setVelocity(unit(v) * 1500.f);
			m_fireCooldown = 10;
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
	}
}
