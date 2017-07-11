#pragma once
#include "Entity.h"
#include "Game/GameCore.h"

class Zombie : public Entity
{
public:
	Zombie(int id);
	void rollback(const NetObject & e);
	void tick(float dt, const NetInput & input, Map & map);
	void preRender(const Snapshot * from, const Snapshot * to, float predictedT, float t);
	void render(sf::RenderTarget & target, Client & client, PlayingScreen & ps);
private:
	ZombieCore m_prevCore;
	ZombieCore m_currentCore;
	float m_aimAngle;
};