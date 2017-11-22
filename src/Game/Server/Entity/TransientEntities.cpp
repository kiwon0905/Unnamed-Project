#include "TransientEntities.h"
#include "Game/NetObject/NetExplosion.h"
#include "Game/Snapshot.h"

Explosion::Explosion(const sf::Vector2f & pos):
	m_position(pos)
{
}

void Explosion::snap(Snapshot & snapshot) const
{
	NetExplosion * ne = static_cast<NetExplosion*>(snapshot.addTransient(NetObject::EXPLOSION));
	if (ne)
	{
		ne->pos.x = (int)std::round(m_position.x * 100.f);
		ne->pos.y = (int)std::round(m_position.y * 100.f);
	}
}
