#include "Player.h"
#include "GameContext.h"
#include "Game//NetObject/NetPlayerInfo.h"
#include "Game/Snapshot.h"
#include "Game/Server/Entity/Character.h"
#include "Core/Server/Server.h"

#include "Game/Server/Entity/Human.h"

Player::Player(int peerId, GameContext * context):
	m_peerId(peerId),
	m_context(context)
{
}

int Player::getPeerId() const
{
	return m_peerId;
}

Character * Player::getCharacter() const
{
	return m_character;
}

void Player::setCharacter(Character * c)
{
	m_character = c;
}

Team Player::getTeam() const
{
	return m_team;
}

void Player::setTeam(Team team)
{
	m_team = team;
}

int Player::getScore() const
{
	return m_score;
}

void Player::setScore(int score)
{
	m_score = score;
}

void Player::addScore(int score)
{
	m_score += score;
}

int Player::getKills() const
{
	return m_kills;
}

void Player::addKill()
{
	++m_kills;
}

int Player::getDeaths() const
{
	return m_deaths;
}

void Player::addDeath()
{
	++m_deaths;
}

int Player::getAssists() const
{
	return m_assists;
}

void Player::addAssist()
{
	++m_assists;
}

void Player::setRespawnTick(int tick)
{
	m_respawnTick = tick;
}

void Player::onInput(int tick, const NetInput & input)
{
	if (!m_character)
		return;
	Input i;
	i.tick = tick;
	i.input = input;
	m_inputs.push(i);
}

NetInput Player::popInput(int tick)
{
	while (!m_inputs.empty() && m_inputs.top().tick < tick)
		m_inputs.pop();

	Input i;
	if (!m_inputs.empty() && m_inputs.top().tick == tick)
	{
		i = m_inputs.top();
		m_inputs.pop();
	}
	return i.input;
}

int Player::getAckTick() const
{
	return m_ackTick;
}

void Player::setAckTick(int tick)
{
	m_ackTick = tick;
}

void Player::reset()
{
	m_character = nullptr;
	m_team = Team::NONE;
	m_score = 0;
	m_kills = 0;
	m_deaths = 0;
	m_assists = 0;
	m_ackTick = -1;
	m_inputs = std::priority_queue<Input, std::vector<Input>, InputComparator>();

}

void Player::tick()
{
	//Waiting for respawn
	if (!m_character && m_respawnTick > 0)
	{
		--m_respawnTick;
		if (m_respawnTick == 0)
		{
			m_character = m_context->getWorld().createEntity<Human>(m_peerId, sf::Vector2f(100.f, 100.f));
			m_respawnTick = -1;
		}
	}
}

void Player::snap(Snapshot & snapshot)
{
	NetPlayerInfo * info = reinterpret_cast<NetPlayerInfo*>(snapshot.addEntity(NetObject::PLAYER_INFO, m_peerId));
	if (info)
	{
		info->type = m_character ? m_character->getNetObjectType() : NetObject::Type::NONE;
		info->id = m_character ? m_character->getId() : -1;
		info->team = m_team;
		info->score = m_score;
		info->kills = m_kills;
		info->deaths = m_deaths;
		info->assists = m_assists;
		info->ping = m_context->getServer()->getPeer(m_peerId)->getENetPeer()->lastRoundTripTime;
		info->respawnTick = m_respawnTick;
	}
}
