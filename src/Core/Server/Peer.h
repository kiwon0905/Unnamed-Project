#pragma once

#include "Core/Packer.h"
#include "Game/NetInput.h"
#include "Game/Enums.h"
#include <enet/enet.h>
#include <queue>

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

	bool send(const Packer & packer, bool reliable);
private:
	int m_id;
	std::string m_name;
	ENetPeer * m_peer;
	State m_state = PRE_GAME;
};