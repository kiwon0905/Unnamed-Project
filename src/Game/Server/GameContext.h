#pragma once
#include "Core/Packer.h"
#include "Core/Protocol.h"

#include "Game/Server/Peer.h"

#include "Game/Server/GameWorld.h"
#include "Game/Enums.h"
#include <enet/enet.h>
#include <memory>

class Server;
class GameContext
{
public:
	GameContext(Server * server);
	

	Server * getServer();
	const Map & getMap() const;
	GameWorld & getWorld();
	int getCurrentTick() const;
	sf::Time getCurrentTime() const;


	virtual std::string getName() = 0;
	

	virtual void prepareRound() = 0;
	void startRound();
	
	//return true if the round needs to be continued
	bool update();
	virtual bool checkRound(Team & team) = 0;
	void reset();
	
	
	//call this in startRound to create crates
	void createCrates();

	//
	void announceDeath(int killedEntity, int killerEntity);
protected:
	Server * m_server;

	sf::Clock m_clock;
	int m_tick = 0;
	sf::Time m_accumulator;
	sf::Time m_prevTime;
	GameWorld m_gameWorld;
	Map m_map;
	SnapshotContainer m_snapshots;
};