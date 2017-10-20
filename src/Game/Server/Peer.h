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
	
	State getState() const;
	void setState(State state);

	Entity * getEntity();
	void setEntity(Entity * e);	
	
	Team getTeam();
	void setTeam(Team team);
	
	int getScore();
	void setScore(int score);
	void addScore(int score);

	void onInput(int tick, const NetInput & input);
	NetInput popInput(int tick);

	void setAckTick(int tick);
	int getAckTick();
	
	void reset();
	bool send(const Packer & packer, bool reliable);
private:
	int m_id;
	std::string m_name;
	ENetPeer * m_peer;
	State m_state = PRE_GAME;

	Entity * m_entity = nullptr;
	Team m_team = Team::NONE;
	int m_score = 0;
	
	int m_ackTick = -1;
	std::priority_queue<Input, std::vector<Input>, InputComparator> m_inputs;
};