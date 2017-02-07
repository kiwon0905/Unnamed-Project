#include "Entity.h"

Entity::Entity(int id, EntityType type):
	m_id(id),
	m_type(type)
{
}

int Entity::getId() const
{
	return m_id;
}

EntityType Entity::getType() const
{
	return m_type;
}

void Entity::setAlive(bool alive)
{
	m_alive = alive;
}

bool Entity::isAlive() const
{
	return !m_alive;
}