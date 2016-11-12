#pragma once

#include "Core/Packet.h"
#include <enet/enet.h>
class Entity;

class Player
{
public:
	Player(ENetPeer * peer);
	ENetPeer * getPeer();
	Entity * getEntity();
	void setEntity(Entity * entity);
	void setInput(sf::Uint8 input);
private:
	Entity * m_entity;
	ENetPeer * m_peer;
	sf::Uint8 m_input;
};