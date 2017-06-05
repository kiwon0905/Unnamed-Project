#pragma once
#include "Game/Enums.h"
#include "Game/GameCore.h"
#include "Game/NetObject.h"
#include "Game/NetInput.h"
#include "Core/Client/Client.h"
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

class GameWorld;
class Snapshot;
class Entity
{
public:
	Entity(int id, EntityType type);
	virtual ~Entity() {}
	
	int getId() const;
	EntityType getType() const;
	
	void setPrediction(bool predict);
	bool isPredicted();
	virtual void rollback(const Snapshot & s) = 0;

	void setAlive(bool alive);
	bool isAlive() const;
	
	const sf::Vector2f & getPosition() const;
	virtual void tick(float dt, const NetInput & input, Map & map) = 0;
	virtual void preRender(const Snapshot * from, const Snapshot * to, float t) = 0;
	virtual void render(sf::RenderTarget & target, Client & client) = 0;
protected:
	int m_id;
	EntityType m_type;
	bool m_alive = true;
	bool m_predicted = false;
	sf::Vector2f m_position;
};
