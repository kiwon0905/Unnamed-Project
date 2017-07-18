#include "TransientEntities.h"
#include "Game/NetObject.h"
#include "Game/Snapshot.h"

Explosion::Explosion(const sf::Vector2f & pos):
	m_position(pos)
{
}

void Explosion::snap(Snapshot & snapshot) const
{
	NetExplosion * exp = static_cast<NetExplosion*>(snapshot.addEvent(NetObject::EXPLOSION));
	if (exp)
	{
	
	}
}
