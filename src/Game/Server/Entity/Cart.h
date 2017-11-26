#pragma once

#include "Game/Server/Entity.h"
#include "Game/Enums.h"

class Cart : public Entity
{
public:
	Cart(int id, GameContext * context, const sf::Vector2f & pos, Team team);

	void tick(float dt);
	void snap(Snapshot & snapshot) const;
private:
	sf::Vector2f m_velocity;
	Team m_team;
};