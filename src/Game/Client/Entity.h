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
class PlayingScreen;

class Entity
{
public:
	Entity(int id, EntityType type, Client & client, PlayingScreen & screen);
	virtual ~Entity() {}
	
	int getId() const;
	EntityType getType() const;
	
	void setAlive(bool alive);
	bool isAlive() const;
	
	virtual sf::Vector2f getCameraPosition(const Snapshot * from, const Snapshot * to, float predictedT, float t) const = 0;
	virtual void render(const Snapshot * from, const Snapshot * to, float predictedT, float t) = 0;
protected:
	int m_id;
	EntityType m_type;
	bool m_alive = true;
	Client * m_client;
	PlayingScreen * m_screen;
};
