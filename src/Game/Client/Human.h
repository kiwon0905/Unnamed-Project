#pragma once

#include "Entity.h"
#include "Game/CharacterCore.h"

class Human : public Entity
{
public:
	Human(int id);
	void update(float dt, GameWorld & world);
	void renderPast(Renderer & renderer, const NetEntity * from, const NetEntity * to, float t);
	void renderFuture(Renderer & renderer, const CharacterCore & core);
	

};