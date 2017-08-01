#pragma once

#include "Core/Packer.h"
#include "Game/NetInput.h"
#include "Game/Enums.h"
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
	void onInput(const NetInput & input);
	NetInput popInput(int tick);
private:
	int m_id;
	std::string m_name;
	ENetPeer * m_peer;
	Entity * m_entity = nullptr;
	State m_state = PRE_GAME;
	Team m_team = Team::NONE;
	std::priority_queue<NetInput, std::vector<NetInput>, InputComparator> m_inputs;
};