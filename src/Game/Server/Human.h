#pragma once
#include "Entity.h"

class Human : public Entity
{
public:
	Human(int id, Peer * player);

	void update(float dt, GameWorld & world);
	void snap(Packer & packer);
private:
};