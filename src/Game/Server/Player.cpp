#include "Player.h"
#include "GameContext.h"
#include "Game/NetObject.h"
#include "Game/Snapshot.h"
#include "Game/Server/Entity.h"
#include "Core/Server/Server.h"

Player::Player(int peerId, GameContext * context):
	m_peerId(peerId),
	m_context(context)
{
}

int Player::getPeerId() const
{
	return m_peerId;
}

Entity * Player::getEntity() const
{
	return m_entity;
}

void Player::setEntity(Entity * e)
{
	m_entity = e;
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

void Player::onInput(int tick, const NetInput & input)
{
	if (!m_entity)
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
	m_entity = nullptr;
	m_team = Team::NONE;
	m_score = 0;
	m_ackTick = -1;
	while (!m_inputs.empty())
		m_inputs.pop();
}

void Player::tick()
{
}

void Player::snap(Snapshot & snapshot)
{
	{
		NetPlayerInfo * info = reinterpret_cast<NetPlayerInfo*>(snapshot.addEntity(NetObject::PLAYER_INFO, m_peerId));
		if (info)
		{
			info->score = m_score;
			info->kills = m_kills;
			info->deaths = m_deaths;
			info->assists = m_assists;
			info->ping = m_context->getServer()->getPeer(m_peerId)->getENetPeer()->lastRoundTripTime;
			info->team = m_team;
			info->type = m_entity ? m_entity->getType() : NetObject::Type::NONE;
			info->id = m_entity ? m_entity->getId() : -1;
		}
	}

}
