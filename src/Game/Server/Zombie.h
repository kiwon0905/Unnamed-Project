#pragma once

#include "Entity.h"
#include "Game/GameCore.h"

class Zombie : public Entity
{
public:
	Zombie(int id, Peer * player);

	void tick(float dt, GameWorld & world);
	void snap(Snapshot & snapshot) const;

	void takeDamage(int dmg);
private:
	ZombieCore m_core;
	int m_health = 30;
};