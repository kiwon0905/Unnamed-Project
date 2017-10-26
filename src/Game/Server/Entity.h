#pragma once
#include "Game/Snapshot.h"
#include "Game/Aabb.h"

#include <SFML/System.hpp>

class GameWorld;
class GameContext;
class Peer;

class Entity
{
public:
	Entity(int id, NetObject::Type type, GameContext * context, const sf::Vector2f & pos = sf::Vector2f());
	virtual ~Entity() {}
	
	int getId();
	NetObject::Type getType();

	bool isAlive();
	void setAlive(bool alive);

	sf::Vector2f getPosition();
	sf::Vector2f getSize();
	
	virtual void tick(float dt) = 0;
	virtual void snap(Snapshot & snapshot) const = 0;
	virtual void takeDamage(int dmg, int from, const sf::Vector2f & impulse);
protected:
	int m_id;
	NetObject::Type m_type;
	bool m_alive = true;
	sf::Vector2f m_position;
	sf::Vector2f m_size;
	GameContext * m_context;
};

class Transient
{
public:
	virtual void snap(Snapshot & snapshot) const = 0;
	virtual ~Transient() {}
};