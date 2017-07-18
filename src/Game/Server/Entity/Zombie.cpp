#include "Zombie.h"
#include "Game/Server/GameContext.h"
#include "Game/Server/GameWorld.h"
#include "Core/Utility.h"
#include "Core/Server/Peer.h"

Zombie::Zombie(int id, GameContext * context, int peerId):
	Entity(id, EntityType::ZOMBIE, context),
	m_peerId(peerId)
{
	m_size = { 69.f, 69.f };
}

void Zombie::tick(float dt)
{
	
	NetInput input = m_context->getPeer(m_peerId)->popInput(m_context->getCurrentTick());

	m_core.tick(dt, input, m_context->getMap());
	m_position = m_core.getPosition();
}

void Zombie::snap(Snapshot & snapshot) const
{
	NetZombie * h = static_cast<NetZombie*>(snapshot.addEntity(NetObject::ZOMBIE, m_id));
	if (h)
	{
		m_core.write(*h);
	}
}

void Zombie::takeDamage(int dmg)
{
	m_health -= dmg;
	if (m_health <= 0)
		m_alive = false;
}
