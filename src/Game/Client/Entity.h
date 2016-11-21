#pragma once
#include "Core/Client/Renderer.h"
#include "Game/GameCore.h"

#include <SFML/System.hpp>

class GameWorld;
class Entity
{
public:
	Entity(unsigned id, EntityType type);

	unsigned getId() const;
	EntityType getType() const;
	void die();
	bool isDead() const;
	virtual void update(float dt, GameWorld & world) = 0;
	virtual void render(Renderer & renderer) = 0;
protected:
	unsigned m_id;
	EntityType m_type;
	bool m_alive = true;
	EntityState m_state;
	sf::Vector2f m_position;
};