#include "Zombie.h"
#include "GameWorld.h"
#include "Core/Utility.h"
#include "Core/Server/Peer.h"

Zombie::Zombie(int id, Peer * player):
	Entity(id, EntityType::ZOMBIE, player)
{
	m_size = { 69.f, 69.f };
}

void Zombie::tick(float dt, GameWorld & world)
{
	NetInput input = m_player->popInput(world.getCurrentTick());

	m_core.tick(dt, input, world.getMap());
	m_position = m_core.getPosition();
}

void Zombie::snap(Snapshot & snapshot) const
{
	NetZombie * h = static_cast<NetZombie*>(snapshot.addEntity(NetObject::ZOMBIE, m_id));
	if (h)
	{
		h->vel.x = roundToInt(m_core.getVelocity().x * 100.f);
		h->vel.y = roundToInt(m_core.getVelocity().y * 100.f);
		h->pos.x = roundToInt(m_core.getPosition().x * 100.f);
		h->pos.y = roundToInt(m_core.getPosition().y * 100.f);
	}
}

void Zombie::takeDamage(int dmg)
{
	m_health -= dmg;
	if (m_health <= 0)
		m_alive = false;
}
