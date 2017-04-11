#pragma once
#include "Game/GameConfig.h"
#include "Game/GameCore.h"
#include "Game/NetObject.h"

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

class GameWorld;
class Entity
{
public:
	Entity(int id, EntityType type);
	virtual ~Entity() {}
	int getId() const;
	EntityType getType() const;
	void setAlive(bool alive);
	bool isAlive() const;
	virtual void renderPast(const NetObject * from, const NetObject * to, float t, sf::RenderTarget & target) = 0;
	virtual void renderFuture(const CharacterCore & prevCore, const CharacterCore & prevCurrent, float t, sf::RenderTarget & target) = 0;
protected:
	int m_id;
	EntityType m_type;
	bool m_alive = true;
};
