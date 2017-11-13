#include "Peer.h"
#include "Game/Snapshot.h"
#include "Game/Server/Entity.h"
#include "Core/ENetUtility.h"


Peer::Peer(int id, ENetPeer * peer) :
	m_id(id),
	m_peer(peer)
{
}

int Peer::getId()
{
	return m_id;
}

const std::string & Peer::getName()
{
	return m_name;
}

void Peer::setName(const std::string & name)
{
	m_name = name;
}

ENetPeer * Peer::getENetPeer() const
{
	return m_peer;
}

Entity * Peer::getEntity()
{
	return m_entity;
}

void Peer::setEntity(Entity * e)
{
	m_entity = e;
}

void Peer::reset()
{
	m_entity = nullptr;
	m_team = Team::NONE;
	m_score = 0;
	m_ackTick = -1;
	while (!m_inputs.empty())
		m_inputs.pop();
}

bool Peer::send(const Packer & packer, bool reliable)
{
	return enutil::send(packer, m_peer, reliable);
}

Peer::State Peer::getState() const
{
	return m_state;
}

void Peer::setState(State state)
{
	m_state = state;
}

Team Peer::getTeam()
{
	return m_team;
}

void Peer::setTeam(Team team)
{
	m_team = team;
}

int Peer::getScore()
{
	return m_score;
}

void Peer::setScore(int score)
{
	m_score = score;
}

void Peer::addScore(int score)
{
	m_score += score;
}

int Peer::getKills()
{
	return m_kills;
}

void Peer::setKills(int kills)
{
	m_kills = kills;
}

void Peer::addKills(int kills)
{
	m_kills += kills;

	std::cout <<m_id<< " kill added\n";
}

int Peer::getDeaths()
{
	return m_deaths;
}

void Peer::setDeaths(int deaths)
{
	m_deaths = deaths;
}

void Peer::addDeaths(int deaths)
{
	m_deaths += deaths;

	std::cout <<m_id<< " death added\n";
}

int Peer::getAssists()
{
	return m_assists;
}

void Peer::setAssists(int assists)
{
	m_assists = assists;
}

void Peer::addAssists(int assists)
{
	m_assists += assists;
}

void Peer::onInput(int tick, const NetInput & input)
{
	Input i;
	i.tick = tick;
	i.input = input;
	m_inputs.push(i);
}

NetInput Peer::popInput(int tick)
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

void Peer::setAckTick(int tick)
{
	m_ackTick = tick;
}

int Peer::getAckTick()
{
	return m_ackTick;
}


void Peer::snap(Snapshot & snapshot)
{
	//if (m_entity)
	{
		NetPlayerInfo * info = reinterpret_cast<NetPlayerInfo*>(snapshot.addEntity(NetObject::PLAYER_INFO, m_id));
		if (info)
		{
			info->score = m_score;
			info->kills = m_kills;
			info->deaths = m_deaths;
			info->assists = m_assists;
			
			info->team = m_team;
			info->type = m_entity ? m_entity->getType() : NetObject::Type::NONE;
			info->id = m_entity ? m_entity->getId() : -1;
		}
	}

}
