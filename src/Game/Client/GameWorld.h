#pragma once
#include "Core/Client/Client.h"
#include "Core/Packer.h"
#include "Human.h"
#include "Game/GameCore.h"

#include <unordered_map>
#include <deque>

class GameWorld
{
public:
	struct Snapshot
	{
		int tick;
		float time;

	};
	struct Input
	{
		int seq;
		unsigned bits;
	};
	
	void onDisconnect();
	void update(float dt, Client & client);
	void render(Client & client);

	void load();
	void onWorldInfo(Unpacker & unpacker, Client & client);
	void onSnapshot(Unpacker & unpacker);

	const std::deque<Snapshot> & getSnapshots();
	const std::deque<Input> & getInputs();
private:
	std::deque<Input> m_inputs;
	std::deque<Snapshot> m_snapshots;
	bool m_ready = false;
	float m_delay = 3.f;
	float m_time = 0.f;
	int m_playerEntityId = -1;

};