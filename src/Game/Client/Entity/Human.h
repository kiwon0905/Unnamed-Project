#pragma once

#include "Game/Client/PredictedEntity.h"
#include "Game/GameCore.h"

class Human : public PredictedEntity
{
public:
	Human(int id, Client & client, PlayingScreen & screen);
	void rollback(const void * e);

	void tick(float dt, const NetInput & input, Map & map);
	sf::Vector2f getCameraPosition(const Snapshot * from, const Snapshot * to, float predictedT, float t) const;
	void render(const Snapshot * from, const Snapshot * to, float predictedT, float t);

private:
	sf::Vector2f getRenderPos(const NetHuman * from, const NetHuman * to, float predictedT, float t) const;
	
	HumanCore m_prevCore;
	HumanCore m_currentCore;
};