#pragma once

#include "Game/Server/Entity.h"
#include "Game/GameCore.h"

class Zombie : public Entity
{
public:
	Zombie(int id, GameContext * context, int peerId, const sf::Vector2f & pos);

	void tick(float dt);
	void snap(Snapshot & snapshot) const;

	void takeDamage(int dmg);
private:
	ZombieCore m_core;
	int m_health = 30;
	int m_peerId = -1;
};