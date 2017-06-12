#pragma once
#include "Game/Enums.h"
#include "Game/Snapshot.h"
#include "Game/Aabb.h"

#include <SFML/System.hpp>

class GameWorld;
class Peer;

class Entity
{
public:
	Entity(int id, EntityType type, Peer * player);
	virtual ~Entity() {}
	
	int getId();
	EntityType getType();
	
	Peer * getPlayer();
	
	bool isAlive();
	void setAlive(bool alive);

	sf::Vector2f getPosition();
	sf::Vector2f getSize();
	Aabb getAabb();

	virtual void tick(float dt, GameWorld & gameWorld) = 0;
	virtual void snap(Snapshot & snapshot) const = 0;

protected:
	int m_id;
	EntityType m_type;
	bool m_alive = true;
	Peer * m_player = nullptr;

	sf::Vector2f m_position;
	sf::Vector2f m_size;
	
};