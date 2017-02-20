#pragma once
#include "Core/Client/Renderer.h"
#include "Game/GameCore.h"
#include "Game/CharacterCore.h"
#include "Game/NetObject.h"

#include <SFML/System.hpp>
#include <deque>

class GameWorld;
class Entity
{
public:
	Entity(int id, EntityType type);

	int getId() const;
	EntityType getType() const;
	void setAlive(bool alive);
	bool isAlive() const;
	virtual void update(float dt, GameWorld & world) = 0;
	virtual void renderPast(Renderer & renderer, const NetEntity * from, const NetEntity * to, float t) = 0;
	virtual void renderFuture(Renderer & renderer, const CharacterCore & prevCore, const CharacterCore & prevCurrent, float t) = 0;
protected:
	int m_id;
	EntityType m_type;
	bool m_alive = true;
	EntityState m_state;
};
