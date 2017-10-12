#pragma once
#include "Game/Server/Entity.h"
#include "Game/GameCore.h"

class Human : public Entity
{
public:
	Human(int id, GameContext * context, int peerId, const sf::Vector2f & pos);

	void tick(float dt);
	void snap(Snapshot & snapshot) const;

	void takeDamage(int dmg, int from, const sf::Vector2f & impulse);
	int getPeerId();
private:
	HumanCore m_core;
	int m_fireCooldown = 0;
	int m_health = 100;
	float m_aimAngle = 0.f;
	int m_peerId = -1;
};