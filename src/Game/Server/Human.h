#pragma once
#include "Entity.h"
#include "Game/GameCore.h"

class Human : public Entity
{
public:
	Human(int id, Peer * player);

	void tick(float dt, GameWorld & world);
	void snap(Snapshot & snapshot) const;

	void takeDamage(int dmg);
private:
	HumanCore m_core;
	int m_fireCooldown = 0;
	int m_health;
	float m_aimAngle = 0.f;
};