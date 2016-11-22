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

void Player::setInputSeq(unsigned seq)
{
	if(seq > m_inputSeq)
		m_inputSeq = seq;
}

unsigned Player::getInputSeq()
{
	return m_inputSeq;
}

bool Player::isReady() const
{
	return m_ready;
}

void Player::setReady(bool ready)
{
	m_ready = ready;
}

