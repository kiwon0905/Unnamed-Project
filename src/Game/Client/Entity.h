#pragma once
#include "Core/Client/Renderer.h"
#include "Game/GameCore.h"

#include <SFML/System.hpp>
#include <deque>

class GameWorld;
class Entity
{
public:
	Entity(unsigned id, EntityType type);

	unsigned getId() const;
	EntityType getType() const;
	void die();
	bool isDead() const;
	void setPrediction(bool predict);
	virtual void update(float dt, GameWorld & world) = 0;
	virtual void render(Renderer & renderer) = 0;
protected:
	struct PastMove
	{
		sf::Vector2f dv;
		unsigned seq;
	};


	bool m_predicted = false;
	std::deque<PastMove> m_pastMoveBuffer;
	unsigned m_id;
	EntityType m_type;
	bool m_alive = true;
	EntityState m_state;
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
};