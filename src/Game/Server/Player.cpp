#include "Player.h"

Player::Player(ENetPeer * peer) :
	m_peer(peer)
{
}

ENetPeer * Player::getPeer()
{
	return m_peer;
}

Entity * Player::getEntity()
{
	return m_entity;
}

void Player::setEntity(Entity * entity)
{
	m_entity = entity;
}

