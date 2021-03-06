#pragma once
#include "Player.h"
#include "Game/Server/GameWorld.h"
#include "Game/Enums.h"

#include "Core/Packer.h"
#include "Core/Protocol.h"
#include "Core/Server/Peer.h"

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

	Player * getPlayer(int peerId);
	void onJoin(int peerId);
	void onDisconnect(int peerId);
	void onInput(int peerId, int predictedTick, const NetInput & input, int ackTick);

	int getCurrentTick() const;

	virtual std::string getName() = 0;
	virtual void loadRound() = 0;
	void startRound();
	virtual void onRoundStart() = 0;

	virtual void tick(float dt) = 0;
	virtual void snap(Snapshot & snapshot) = 0;
	//return true if the round needs to be continued
	bool update();

	virtual bool checkRound(Team & team) = 0;
	virtual void reset();
	
	virtual void onCharacterDeath(int killedPeer, int killerPeer, const std::unordered_map<int, int> & assisters);
	void addScore(int peerId, int score);
private:
protected:
	Server * m_server;

	sf::Clock m_clock;
	int m_tick = 0;
	sf::Time m_accumulator;
	sf::Time m_prevTime;
	
	std::vector<Player> m_players;
	GameWorld m_gameWorld;
	Map m_map;
	SnapshotContainer m_snapshots;
};