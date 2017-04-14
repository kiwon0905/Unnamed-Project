#include "Human.h"
#include "Projectile.h"
#include "GameWorld.h"
#include "Game/GameConfig.h"
#include "Game/Control.h"
#include "Game/NetObject.h"
#include "Core/Utility.h"
#include "Core/Server/Peer.h"

Human::Human(int id, Peer * player):
	Entity(id, EntityType::HUMAN, player)
{
}

void Human::tick(float dt, GameWorld & world)
{
	Input input = m_player->popInput(world.getCurrentTick());

	m_core.tick(dt, input.bits, world.getMap());
	if (input.bits & Control::PRIMARY_FIRE)
	{
		if (m_fireCooldown == 0)
		{
			Projectile * p = static_cast<Projectile*>(world.createEntity(EntityType::PROJECTILE));
			p->setPosition(m_core.getPosition());
			p->setVelocity(m_core.getVelocity());
			m_fireCooldown = 10;
		}
	}

	m_fireCooldown--;
	if (m_fireCooldown < 0)
		m_fireCooldown = 0;
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
