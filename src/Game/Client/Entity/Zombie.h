#pragma once
#include "Game/Client/PredictedEntity.h"
#include "Game/GameCore.h"

class Zombie : public PredictedEntity
{
public:
	Zombie(int id, Client & client, PlayingScreen & screen);

	bool rollback(Snapshot & snapshot);

	void tick(float dt, const NetInput & input, Map & map);


	const void * find(const Snapshot & s);

	sf::Vector2f getCameraPosition(const Snapshot * from, const Snapshot * to, float predictedT, float t) const;
	void render(const Snapshot * from, const Snapshot * to, float predictedT, float t);

	const ZombieCore & getCore() const;
private:
	sf::Vector2f getRenderPos(const NetZombie * from, const NetZombie * to, float predictedT, float t) const;
	ZombieCore m_prevCore;
	ZombieCore m_currentCore;
};