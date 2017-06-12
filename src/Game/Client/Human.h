#pragma once

#include "Entity.h"
#include "Game/GameCore.h"

class Human : public Entity
{
public:
	Human(int id);
	void rollback(const NetObject & e);
	void tick(float dt, const NetInput & input, Map & map);
	void preRender(const Snapshot * from, const Snapshot * to, float t);
	void render(sf::RenderTarget & target, Client & client);

private:
	HumanCore m_prevCore;
	HumanCore m_currentCore;
	float m_aimAngle;
};