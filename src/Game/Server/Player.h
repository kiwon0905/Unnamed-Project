#pragma once

#include "Core/Packer.h"
#include <enet/enet.h>
#include <deque>
class Entity;

struct Input
{
	unsigned bits;
	int seq;
};
class Player
{
public:
	Player(ENetPeer * peer);
	ENetPeer * getPeer() const;
	Entity * getEntity() const;
	void setEntity(Entity * entity);

	void onInput(unsigned bits, int seq);
	Input * peekInput();
	void popInput();
	unsigned getLastProcessedInputSeq();

	bool isReady() const;
	void setReady(bool ready);
private:
	Entity * m_entity;
	ENetPeer * m_peer;
	bool m_ready = false;
	std::deque<Input> m_inputs;
	int m_lastProcessedInputSeq = 0;
};