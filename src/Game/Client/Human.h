#pragma once

#include "Entity.h"
#include "Game/GameCore.h"

class Human : public Entity
{
public:
	Human(int id);
	void renderPast(const NetObject * from, const NetObject * to, float t, sf::RenderTarget & target);
	void renderFuture(const CharacterCore & prevCore, const CharacterCore & prevCurrent, float t, sf::RenderTarget & target);
	

};