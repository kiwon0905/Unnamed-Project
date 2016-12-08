#include "Player.h"

Player::Player(ENetPeer * peer) :
	m_peer(peer)
{
}

ENetPeer * Player::getPeer() const
{
	return m_peer;
}

Entity * Player::getEntity() const
{
	return m_entity;
}

void Player::setEntity(Entity * character)
{
	m_entity = character;
}

void Player::onInput(unsigned bits, int seq)
{
	if (seq > m_lastProcessedInputSeq)
	{
		Input input;
		input.bits = bits;
		input.seq = seq;
		m_inputs.push_back(input);

	}
}

Input * Player::peekInput()
{
	if(m_inputs.empty())
		return nullptr;
	return &m_inputs.front();
}

void Player::popInput()
{
	m_lastProcessedInputSeq = m_inputs.front().seq;
	m_inputs.pop_front();
}

unsigned Player::getLastProcessedInputSeq()
{
	return m_lastProcessedInputSeq;
}

bool Player::isReady() const
{
	return m_ready;
}

void Player::setReady(bool ready)
{
	m_ready = ready;
}

