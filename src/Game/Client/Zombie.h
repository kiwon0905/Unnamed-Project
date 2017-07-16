#pragma once
#include "Entity.h"
#include "Game/GameCore.h"

class Zombie : public Entity
{
public:
	Zombie(int id, Client & client, PlayingScreen & screen);
	void rollback(const NetObject & e);
	void tick(float dt, const NetInput & input, Map & map);

	sf::Vector2f getCameraPosition(const Snapshot * from, const Snapshot * to, float predictedT, float t) const;
	void render(const Snapshot * from, const Snapshot * to, float predictedT, float t);

private:
	sf::Vector2f getRenderPos(const NetZombie * from, const NetZombie * to, float predictedT, float t) const;
	ZombieCore m_prevCore;
	ZombieCore m_currentCore;
};