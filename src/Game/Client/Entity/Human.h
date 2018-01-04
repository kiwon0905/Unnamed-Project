#pragma once

#include "Game/Client/PredictedEntity.h"
#include "Game/GameCore.h"

#include "Core/Client/Animation.h"
#include "Core/Client/AnimatedSprite.h"

class Human : public PredictedEntity
{
public:
	Human(int id, Client & client, PlayingScreen & screen);
	bool rollback(Snapshot & snapshot);

	const void * find(const Snapshot & s);

	void tick(float dt, const NetInput & input, Map & map);
	sf::Vector2f getCameraPosition() const;
	void render();

private:
	sf::Vector2f getRenderPos(const NetHuman * from, const NetHuman * to) const;
	
	HumanCore m_prevCore;
	HumanCore m_currentCore;

	Animation m_restAnimation;
	Animation m_walkingAnimation;
	Animation m_inAirAnimation;
	AnimatedSprite m_sprite;

	sf::RectangleShape m_body;
	sf::RectangleShape m_gun;
	sf::RectangleShape m_healthBackground;
	sf::RectangleShape m_health;
	sf::Text m_name;
};