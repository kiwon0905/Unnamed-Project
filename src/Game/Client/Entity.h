#pragma once
#include "Game/GameConfig.h"
#include "Game/CharacterCore.h"
#include "Game/NetObject.h"

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

class GameWorld;
class Entity
{
public:
	Entity(int id, EntityType type);
	virtual ~Entity() { std::cout << "I'm destroyed!\n"; }
	int getId() const;
	EntityType getType() const;
	void setAlive(bool alive);
	bool isAlive() const;
	virtual void update(float dt, GameWorld & world) = 0;
	virtual void renderPast(const NetItem * from, const NetItem * to, float t, sf::RenderTarget & target) = 0;
	virtual void renderFuture(const CharacterCore & prevCore, const CharacterCore & prevCurrent, float t) = 0;
protected:
	int m_id;
	EntityType m_type;
	bool m_alive = true;
};
