#pragma once

#include "Core/Packer.h"
#include "Game/NetInput.h"
#include <enet/enet.h>
#include <queue>
class Entity;

struct InputComparator
{
	bool operator()(const NetInput & i, const NetInput & j)
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
		IN_GAME
	};

	Peer(int id, ENetPeer * peer);
	int getId();
	ENetPeer * getENetPeer() const;
	Entity * getEntity();
	void setEntity(Entity * e);
	bool send(Packer & packer, bool reliable);
	State getState() const;
	void setState(State state);
	
	void onInput(const NetInput & input);
	NetInput popInput(int tick);
private:
	int m_id;
	ENetPeer * m_peer;
	Entity * m_entity = nullptr;
	State m_state = PRE_GAME;
	std::priority_queue<NetInput, std::vector<NetInput>, InputComparator> m_inputs;
};