#include "Crate.h"
#include "Game/NetObject.h"
#include "Game/Server/GameContext.h"
#include "Core/Utility.h"


Crate::Crate(int id, GameContext * context, const sf::Vector2f & pos):
	Entity(id, NetObject::CRATE, context)
{
	m_position = pos;
	m_size = { 70.f, 70.f };
	m_health = 3;
}

void Crate::tick(float dt)
{

}

void Crate::snap(Snapshot & snapshot) const
{
	NetCrate * c = static_cast<NetCrate*>(snapshot.addEntity(NetObject::CRATE, m_id));
	if (c)
	{
		c->pos.x = Math::roundToInt(m_position.x * 100.f);
		c->pos.y = Math::roundToInt(m_position.y * 100.f);

	}
}

void Crate::takeDamage(int dmg, int from, const sf::Vector2f & impulse)
{
	m_health--;
	if(m_health <= 0)
		m_alive = false;

}
