#pragma once

#include "Game/Server/Entity/Character.h"
#include "Game/GameCore.h"

class Zombie : public Character
{
public:
	Zombie(int id, GameContext * context, int peerId, const sf::Vector2f & pos);

	void tick(float dt);
	void snap(Snapshot & snapshot) const;

	void takeDamage(int dmg, int from, const sf::Vector2f & impulse);
private:
	ZombieCore m_core;
	int m_fireCooldown = 0;
	float m_aimAngle = 0.f;
};