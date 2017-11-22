#pragma once
#include "Game/Enums.h"
#include "Game/NetInput.h"
#include "Core/Server/Peer.h"
#include <queue>

class Character;
class Snapshot;
class GameContext;

class Player
{
public:
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

	Player(int peerId, GameContext * context);

	int getPeerId() const;

	Character * getCharacter() const;
	void setCharacter(Character * c);

	Team getTeam() const;
	void setTeam(Team team);

	int getScore() const;
	void setScore(int score);
	void addScore(int score);

	int getKills() const;
	void addKill();

	int getDeaths() const;
	void addDeath();

	int getAssists() const;
	void addAssist();

	void setRespawnTick(int tick);

	void onInput(int tick, const NetInput & input);
	NetInput popInput(int tick);

	int getAckTick() const;
	void setAckTick(int tick);

	void reset();
	void tick();
	void snap(Snapshot & snapshot);
private:
	int m_peerId;
	GameContext * m_context;
	Character * m_character = nullptr;
	Team m_team = Team::NONE;
	int m_score = 0;
	int m_kills = 0;
	int m_deaths = 0;
	int m_assists = 0;
	int m_respawnTick = 0; //Tick untill respawn if dead(m_entity == nullptr)

	int m_ackTick = -1;
	std::priority_queue<Input, std::vector<Input>, InputComparator> m_inputs;
};