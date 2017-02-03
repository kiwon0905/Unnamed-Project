#pragma once
#include "Core/Client/Renderer.h"
#include "Game/GameCore.h"

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
	sf::Vector2f getPosition();
	void setPosition(sf::Vector2f v);
	virtual void update(float dt, GameWorld & world) = 0;
	virtual void render(Renderer & renderer) = 0;
protected:
	int m_id;
	EntityType m_type;
	bool m_alive = true;
	EntityState m_state;
	sf::Vector2f m_position;
};