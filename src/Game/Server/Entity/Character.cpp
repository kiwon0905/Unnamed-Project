#include "Character.h"
#include "Game/Server/GameContext.h"
#include "Game/Server/Player.h"

Character::Character(int id, GameContext * context, int peerId, const sf::Vector2f & pos):
	Entity(id, EntityType::CHARACTER, context, pos),
	m_peerId(peerId)
{
}

int Character::getPeerId() const
{
	return m_peerId;
}

void Character::tick(float dt)
{
	//remove old assisters
	for (auto it = m_assistingPeers.begin(); it != m_assistingPeers.end();)
	{
		it->second++;
		if (it->second > TICKS_PER_SEC * 5)
			it = m_assistingPeers.erase(it);
		else
			++it;
	}

}

void Character::takeDamage(int dmg, int from, const sf::Vector2f & impulse)
{
	m_assistingPeers[from] = 0;

	m_health -= dmg;
	if (m_health <= 0)
	{
		die(from);
	}
}

void Character::die(int killerPeerId)
{
	m_alive = false;

	//remove killer from the assisters
	m_assistingPeers.erase(killerPeerId);

	m_context->announceDeath(m_peerId, killerPeerId, m_assistingPeers);
	m_context->addScore(killerPeerId, 5);

	Player * player = m_context->getPlayer(m_peerId);
	player->setCharacter(nullptr);
}

NetInput Character::getInput()
{
	return m_context->getPlayer(m_peerId)->popInput(m_context->getCurrentTick());
}

Player * Character::getPlayer()
{
	return m_context->getPlayer(m_peerId);
}
