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

bool Player::isReady() const
{
	return m_ready;
}

void Player::setReady(bool ready)
{
	m_ready = ready;
}

