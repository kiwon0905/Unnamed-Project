#pragma once

#include "Game/Server/Entity.h"
#include "Game/NetInput.h"

class Player;

class Character : public Entity
{
public:
	Character(int id, GameContext * context, int peerId, const sf::Vector2f & pos);

	int getPeerId() const;

	virtual void tick(float dt);

	virtual void takeDamage(int dmg, int from, const sf::Vector2f & impulse);

	virtual NetObject::Type getNetObjectType() = 0;
protected:
	NetInput getInput();
	Player * getPlayer();

	int m_peerId;
	int m_health = 100;
	std::unordered_map<int, int> m_assistingPeers;
};