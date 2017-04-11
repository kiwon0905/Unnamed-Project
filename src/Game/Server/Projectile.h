#pragma once

#include "Entity.h"

class Projectile : public Entity
{
public:
	Projectile(int id);

	void tick(float dt, GameWorld & gameWorld);
	void snap(Snapshot & snapshot) const;

	void setPosition(sf::Vector2f v);
	void setVelocity(sf::Vector2f v);
private:
	sf::Vector2f m_velocity;
	sf::Vector2f m_position;
};