#include "Human.h"
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
}

void Human::snap(Snapshot & snapshot) const
{
	NetHuman * h = static_cast<NetHuman*>(snapshot.addEntity(NetItem::HUMAN, m_id));
	if (h)
	{
		h->velocity = m_core.getVelocity();
		h->position = m_core.getPosition();
	}
}
