#include "Entity.h"
#include "Game/Control.h"

Entity::Entity(unsigned id, EntityType type, Player * player):
	m_id(id),
	m_type(type),
	m_player(player)
{
}

unsigned Entity::getId()
{
	return m_id;
}

Player * Entity::getPlayer()
{
	return m_player;
}

void Entity::die()
{
	m_alive = false;
}

bool Entity::isDead()
{
	return !m_alive;
}

sf::Vector2f Entity::getPosition()
{
	return m_position;
}
