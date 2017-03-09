#include "Entity.h"
#include "Game/Control.h"

Entity::Entity(int id, EntityType type, Peer * player):
	m_id(id),
	m_type(type),
	m_player(player)
{
}

int Entity::getId()
{
	return m_id;
}

EntityType Entity::getType()
{
	return m_type;
}

Peer * Entity::getPlayer()
{
	return m_player;
}

bool Entity::isAlive()
{
	return m_alive;
}

void Entity::setAlive(bool alive)
{
	m_alive = alive;
}
