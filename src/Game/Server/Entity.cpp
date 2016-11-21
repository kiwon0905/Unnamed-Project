#include "Entity.h"
#include "Game/Control.h"

Entity::Entity(unsigned id, EntityType type, Player * player):
	m_id(id),
	m_type(type),
	m_player(player)
{
}

void Entity::onInput(unsigned inputBits)
{
	Input input;
	if (inputBits & Control::MOVE_LEFT)
	{
		input.direction--;
	}
	if (inputBits & Control::MOVE_RIGHT)
	{
		input.direction++;
	}
	if (inputBits & Control::JUMP)
	{
		input.jump = true;
	}
	m_inputs.push(input);
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
