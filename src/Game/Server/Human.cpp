#include "Human.h"
#include "GameWorld.h"
#include "Game/GameCore.h"
#include "Game/Control.h"
#include "Game/NetObject.h"
#include "Core/Utility.h"
#include "Core/Server/Peer.h"

Human::Human(int id, Peer * player):
	Entity(id, EntityType::HUMAN, player)
{
}

void Human::update(float dt, GameWorld & world)
{
	Input input = m_player->popInput();
	m_core.update(dt, input.bits, world.getMap());
}

void Human::sync(Packer & packer)
{
	NetHuman human;
	human.position = m_core.getPosition();
	human.velocity = m_core.getVelocity();;
	human.pack(packer);
}
