#include "Entity.h"

Entity::Entity(sf::Uint32 id, Player * player):
	m_id(id),
	m_player(player)
{
}

void Entity::update(float dt, GameWorld & gameWorld)
{
}

void Entity::die()
{
	m_dead = true;
}

bool Entity::isDead()
{
	return m_dead;
}
