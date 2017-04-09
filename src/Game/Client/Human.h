#pragma once

#include "Entity.h"
#include "Game/CharacterCore.h"

class Human : public Entity
{
public:
	Human(int id);
	void update(float dt, GameWorld & world);
	void renderPast(const NetItem * from, const NetItem * to, float t, sf::RenderTarget & target);
	void renderFuture(const CharacterCore & prevCore, const CharacterCore & prevCurrent, float t);
	

};