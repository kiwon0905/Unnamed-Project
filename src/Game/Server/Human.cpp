#include "Human.h"
#include "Game/GameCore.h"
#include "Game/Control.h"
#include "Core/MathUtility.h"

Human::Human(int id, Peer * player):
	Entity(id, EntityType::HUMAN, player)
{
}

void Human::update(float dt, GameWorld & world)
{

}

void Human::sync(Packer & packer)
{
	//id,type,pos
	packer.pack<ENTITY_ID_MIN, ENTITY_ID_MAX>(m_id);
	packer.pack(m_type);
	packer.pack<2>(0.f, 5000.f, m_position.x);
	packer.pack<2>(0.f, 5000.f, m_position.y);
}
