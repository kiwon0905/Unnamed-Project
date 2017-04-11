#pragma once
#include "Entity.h"

class Projectile : public Entity
{
public:
	Projectile(int id);
	void renderPast(const NetObject * from, const NetObject * to, float t, sf::RenderTarget & target);
	void renderFuture(const CharacterCore & prevCore, const CharacterCore & prevCurrent, float t, sf::RenderTarget & target);
};