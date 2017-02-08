#include "Human.h"
#include "Game/GameCore.h"
#include "Game/Control.h"
#include "Game/NetObject.h"
#include "Core/MathUtility.h"
#include "Core/Server/Peer.h"

Human::Human(int id, Peer * player):
	Entity(id, EntityType::HUMAN, player)
{
}

void Human::update(float dt, GameWorld & world)
{
	m_core.update(dt, m_player->getInput().bits);
	m_position = m_core.getPosition();
}

void Human::sync(Packer & packer)
{
	//id,type,pos


	NetHuman human;
	human.position = m_position;
	human.pack(packer);
}
