#pragma once

#include "Core/Packer.h"
#include "Game/NetInput.h"
#include "Game/Enums.h"
#include <enet/enet.h>
#include <queue>
class Entity;

struct Input
{
	int tick;
	NetInput input;
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
		IN_GAME
	};

	Peer(int id, ENetPeer * peer);
	int getId();

	const std::string & getName();
	void setName(const std::string & name);

	ENetPeer * getENetPeer() const;
	Entity * getEntity();
	void setEntity(Entity * e);
	bool send(const Packer & packer, bool reliable);
	State getState() const;
	void setState(State state);
	Team getTeam();
	void setTeam(Team team);
	void onInput(int tick, const NetInput & input);
	NetInput popInput(int tick);

	void setAckTick(int tick);
	int getAckTick();
private:
	int m_id;
	std::string m_name;
	ENetPeer * m_peer;
	Entity * m_entity = nullptr;
	State m_state = PRE_GAME;
	Team m_team = Team::NONE;
	int m_ackTick = -1;
	std::priority_queue<Input, std::vector<Input>, InputComparator> m_inputs;
};