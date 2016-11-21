#pragma once
#include <SFML/System.hpp>
#include "Core/Packer.h"
#include "Game/GameCore.h"
#include <queue>
class GameWorld;
class Player;

class Entity
{
public:
	Entity(unsigned id, EntityType type, Player * player);
	virtual void update(float dt, GameWorld & gameWorld) = 0;
	virtual void snap(Packer & packer) = 0;
	void onInput(unsigned inputBits);
	unsigned getId();
	Player * getPlayer();
	void die();
	bool isDead();
protected:
	struct Input
	{
		int direction = 0;
		bool jump = false;
	};

	unsigned m_id;
	EntityType m_type;
	bool m_alive = true;
	EntityState m_state;
	Player * m_player;
	std::queue<Input> m_inputs;

	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
};