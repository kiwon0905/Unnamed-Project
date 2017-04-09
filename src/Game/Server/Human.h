#pragma once
#include "Entity.h"
#include "Game/CharacterCore.h"

class Human : public Entity
{
public:
	Human(int id, Peer * player);

	void tick(float dt, GameWorld & world);
	void snap(Snapshot & snapshot) const;
private:
	HumanCore m_core;
};