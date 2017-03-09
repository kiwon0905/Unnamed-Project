#pragma once
#include <SFML/System.hpp>
#include "Core/Packer.h"
#include "Game/GameCore.h"

class GameWorld;
class Peer;

class Entity
{
public:
	Entity(int id, EntityType type, Peer * player);
	
	int getId();
	EntityType getType();
	Peer * getPlayer();
	bool isAlive();
	void setAlive(bool alive);

	virtual void update(float dt, GameWorld & gameWorld) = 0;
	virtual void sync(Packer & packer) = 0;
protected:
	int m_id;
	EntityType m_type;
	bool m_alive = true;
	EntityState m_state;
	Peer * m_player = nullptr;
};