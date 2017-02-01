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
	void onSnapshot(Unpacker & unpacker, Client & client);

	const std::deque<Snapshot> & getSnapshots();
	const std::deque<Input> & getInputs();
private:
	void addEntity(Entity * e);

	std::deque<Input> m_inputs;
	std::deque<Snapshot> m_snapshots;
	std::vector<std::unique_ptr<Entity>> m_entities;
	bool m_ready = false;
	bool m_loaded = false;
	float m_delay = 3.f;
	int m_playerEntityId = -1;
	sf::Clock m_lastSnapshot;
};