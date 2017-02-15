#pragma once

#include "Core/Packer.h"
#include <enet/enet.h>
#include <deque>
#include <queue>
class Entity;

struct Input
{
	unsigned bits;
	int tick;
};

struct InputComparator
{
	bool operator()(const Input & i, const Input & j)
	{
		return i.tick > j.tick;
	}
};

class Peer
{
public:
	enum State
	{
		PRE_GAME,
		LOADING,
		ENTERING,
		PLAYING
	};

	Peer(int id, ENetPeer * peer);
	int getId();
	ENetPeer * getENetPeer() const;
	Entity * getEntity();
	void setEntity(Entity * e);
	bool send(Packer & packer, bool reliable);
	State getState() const;
	void setState(State state);
	void onInput(unsigned bits, int seq);
	Input popInput();
	int getLastUsedInputSeq();
private:
	int m_id;
	ENetPeer * m_peer;
	Entity * m_entity = nullptr;
	State m_state = PRE_GAME;
	Input m_lastInput;
	std::priority_queue<Input, std::vector<Input>, InputComparator> m_inputs;
};