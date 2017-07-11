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

void Peer::onInput(const NetInput & input)
{
	m_inputs.push(input);
}

NetInput Peer::popInput(int tick)
{
	while (!m_inputs.empty() && m_inputs.top().tick < tick)
		m_inputs.pop();

	NetInput i;
	if (!m_inputs.empty() && m_inputs.top().tick == tick)
	{
		i = m_inputs.top();
		m_inputs.pop();
	}
	return i;
}

