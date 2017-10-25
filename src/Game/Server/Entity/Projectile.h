#pragma once

#include "Game/Server/Entity.h"

class Projectile : public Entity
{
public:
	Projectile(int id, GameContext * context, int shooterPeerId, Team shooterTeam);

	void tick(float dt);
	void snap(Snapshot & snapshot) const;

	void setPosition(sf::Vector2f v);
	void setVelocity(sf::Vector2f v);
private:
	sf::Vector2f m_velocity;
	int m_shooterPeerId;
	Team m_team;
};