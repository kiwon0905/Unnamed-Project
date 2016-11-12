#pragma once
#include <SFML/System.hpp>

class GameWorld;
class Player;

class Entity
{
public:
	Entity(sf::Uint32 id, Player * player);
	void update(float dt, GameWorld & gameWorld);
	void die();
	bool isDead();
private:
	sf::Uint32 m_id;
	Player * m_player;
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
	bool m_dead = false;

};