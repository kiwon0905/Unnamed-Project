#pragma once

#include "Entity.h"
#include "Game/GameCore.h"

class Human : public Entity
{
public:
	Human(int id);
	void rollback(const NetObject & e);

	void tick(float dt, const NetInput & input, Map & map);
	sf::Vector2f getCameraPosition(const Snapshot * from, const Snapshot * to, float predictedT, float t) const;
	void render(sf::RenderTarget & target, Client & client, PlayingScreen & ps, const Snapshot * from, const Snapshot * to, float predictedT, float t);

private:
	sf::Vector2f getRenderPos(const NetHuman * from, const NetHuman * to, float predictedT, float t) const;
	
	HumanCore m_prevCore;
	HumanCore m_currentCore;
};