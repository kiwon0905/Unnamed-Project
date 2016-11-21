#pragma once
#include "Entity.h"

class Human : public Entity
{
public:
	Human(unsigned id, Player * player);

	void update(float dt, GameWorld & world);
	void snap(Packer & packer);
private:
};