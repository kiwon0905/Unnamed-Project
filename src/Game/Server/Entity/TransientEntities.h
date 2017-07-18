#pragma once

#include "Game/Server/Entity.h"

class Explosion : public TransientEntity
{
public:
	Explosion(const sf::Vector2f & pos);
	
	void snap(Snapshot & snapshot) const;

private:
	sf::Vector2f m_position;
};