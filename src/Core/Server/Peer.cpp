#include "Peer.h"
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

