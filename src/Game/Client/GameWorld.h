#pragma once
#include "Human.h"
#include "Game/GameCore.h"
#include "Game/CharacterCore.h"
#include "Core/Client/Client.h"
#include "Core/Packer.h"

#include <unordered_map>
#include <deque>

class GameWorld
{
public:
	struct Snapshot
	{
		struct EntityInfo
		{
			int id;
			EntityType type;
			std::vector<char> data;
		};
		int tick;
		std::vector<EntityInfo> info;
	};
	struct Input
	{
		int seq;
		unsigned bits;
	};
	
	GameWorld();

	void onDisconnect();
	void update(float dt, Client & client);
	void render(Client & client);

	void load();
	void onWorldInfo(Unpacker & unpacker, Client & client);
	void onSnapshot(Unpacker & unpacker, Client & client);

	const std::deque<Snapshot> & getSnapshots();
	const std::deque<Input> & getInputs();
private:
	Entity * createEntity(int id, EntityType type);
	Entity * getEntity(int id, EntityType type);


	std::deque<Input> m_inputs;
	std::deque<Snapshot> m_snapshots;
	std::vector<std::vector<std::unique_ptr<Entity>>> m_entitiesByType;
	bool m_ready = false;
	bool m_loaded = false;
	float m_delay = .1f;
	int m_playerEntityId = -1;
	sf::Clock m_lastSnapshot;
};