#include "Entity.h"

Entity::Entity(int id, EntityType type, GameContext * context):
	m_id(id),
	m_type(type),
	m_context(context)
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

bool Entity::isAlive()
{
	return m_alive;
}

void Entity::setAlive(bool alive)
{
	m_alive = alive;
}

sf::Vector2f Entity::getPosition()
{
	return m_position;
}

sf::Vector2f Entity::getSize()
{
	return m_size;
}

Aabb Entity::getAabb()
{
	return Aabb(m_position, m_size);
}
