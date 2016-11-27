#include "Entity.h"

Entity::Entity(unsigned id, EntityType type):
	m_id(id),
	m_type(type)
{
}

unsigned Entity::getId() const
{
	return m_id;
}

EntityType Entity::getType() const
{
	return m_type;
}

void Entity::die()
{
	m_alive = false;
}

bool Entity::isDead() const
{
	return !m_alive;
}

void Entity::setPrediction(bool predict)
{
	m_predicted = predict;
}

void Entity::setPosition(sf::Vector2f v)
{
	m_position = v;
}

sf::Vector2f Entity::getPosition()
{
	return m_position;
}
