#pragma once

#include "Core/Packer.h"
#include <enet/enet.h>
class Entity;

class Player
{
public:
	Player(ENetPeer * peer);
	ENetPeer * getPeer() const;
	Entity * getEntity() const;
	void setEntity(Entity * entity);
	void setInputSeq(unsigned seq);
	unsigned getInputSeq();

	bool isReady() const;
	void setReady(bool ready);
private:
	Entity * m_entity;
	ENetPeer * m_peer;
	bool m_ready = false;
	unsigned m_inputSeq;
};