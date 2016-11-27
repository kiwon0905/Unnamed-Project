#pragma once
#include <SFML/System.hpp>
#include "Core/Packer.h"
#include "Game/GameCore.h"
class GameWorld;
class Player;

class Entity
{
public:
	Entity(unsigned id, EntityType type, Player * player);
	virtual void update(float dt, GameWorld & gameWorld) = 0;
	virtual void snap(Packer & packer) = 0;
	unsigned getId();
	Player * getPlayer();
	void die();
	bool isDead();
	sf::Vector2f getPosition();
protected:
	unsigned m_id;
	EntityType m_type;
	bool m_alive = true;
	EntityState m_state;
	Player * m_player;

	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
};