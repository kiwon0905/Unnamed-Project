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

bool Peer::send(Packer & packer, bool reliable)
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

void Peer::onInput(unsigned bits, int seq)
{
	m_input.bits = bits;
	m_input.seq = seq;
}

const Input & Peer::getInput() const
{
	return m_input;
}

