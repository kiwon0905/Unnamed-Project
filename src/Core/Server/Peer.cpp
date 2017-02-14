#include "Peer.h"
#include "Core/ENetUtility.h"

Peer::Peer(int id, ENetPeer * peer) :
	m_id(id),
	m_peer(peer)
{
	m_lastInput.bits = 0;
	m_lastInput.seq = -1;
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
	if (!m_inputs.empty() && m_lastInput.seq >= seq)
		return;
	Input input;
	input.bits = bits;
	input.seq = seq;
	m_inputs.push(input);
}

Input Peer::popInput()
{
	if (!m_inputs.empty())
	{
		m_lastInput = m_inputs.top();
		m_inputs.pop();
	}
	else
	{
		m_lastInput.seq++;
		std::cout << "skipping input!";
	}

	return m_lastInput;
}

int Peer::getLastUsedInputSeq()
{
	return m_lastInput.seq;
}

