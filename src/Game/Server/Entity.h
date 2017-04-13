#pragma once
#include "Game/GameConfig.h"
#include "Game/Snapshot.h"

#include <SFML/System.hpp>

class GameWorld;
class Peer;

class Entity
{
public:
	Entity(int id, EntityType type, Peer * player);
	virtual ~Entity() {}
	
	int getId();
	EntityType getType();
	
	Peer * getPlayer();
	
	bool isAlive();
	void setAlive(bool alive);

	virtual void tick(float dt, GameWorld & gameWorld) = 0;
	virtual void snap(Snapshot & snapshot) const = 0;
protected:
	int m_id;
	EntityType m_type;
	bool m_alive = true;
	Peer * m_player = nullptr;
};