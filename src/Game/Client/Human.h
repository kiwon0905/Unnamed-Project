#pragma once

#include "Entity.h"
#include "Game/GameCore.h"

class Human : public Entity
{
public:
	Human(int id);
	void rollback(const Snapshot & s);
	void tick(float dt, unsigned input, Map & map);
	void render(const Snapshot * from, const Snapshot * to, float t, sf::RenderTarget & target);

private:
	HumanCore m_prevCore;
	HumanCore m_currentCore;
};