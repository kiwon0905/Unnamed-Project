#pragma once

#include "Entity.h"

class Human : public Entity
{
public:
	Human(unsigned id);
	void update(float dt, GameWorld & world);
	void render(Renderer & render);
private:
};