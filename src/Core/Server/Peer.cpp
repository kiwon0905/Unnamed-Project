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

Peer::State Peer::getState() const
{
	return m_state;
}

void Peer::setState(State state)
{
	m_state = state;
}

bool Peer::send(const Packer & packer, bool reliable)
{
	return enutil::send(packer, m_peer, reliable);
}