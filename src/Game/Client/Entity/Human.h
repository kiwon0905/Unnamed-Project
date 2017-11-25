#pragma once

#include "Game/Client/PredictedEntity.h"
#include "Game/GameCore.h"

class Human : public PredictedEntity
{
public:
	Human(int id, Client & client, PlayingScreen & screen);
	bool rollback(Snapshot & snapshot);

	const void * find(const Snapshot & s);


	void tick(float dt, const NetInput & input, Map & map);
	sf::Vector2f getCameraPosition(const Snapshot * from, const Snapshot * to, float predictedT, float t) const;
	void render(const Snapshot * from, const Snapshot * to, float predictedT, float t);

private:
	sf::Vector2f getRenderPos(const NetHuman * from, const NetHuman * to, float predictedT, float t) const;
	
	HumanCore m_prevCore;
	HumanCore m_currentCore;
};