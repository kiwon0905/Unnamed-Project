#pragma once
#include "Human.h"
#include "Game/GameCore.h"
#include "Game/CharacterCore.h"
#include "Core/Client/Client.h"
#include "Core/Packer.h"
#include "Game/NetObject.h"

#include <unordered_map>
#include <deque>

class GameWorld
{
public:
	struct Snapshot
	{
		int tick;
		std::unordered_map<int, std::unique_ptr<NetEntity>> m_entities;
	};
	struct Input
	{
		int tick;
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
	CharacterCore * createCharacterCore(EntityType type);

	bool m_ready = false;
	float m_delay = .1f;

	int m_lastAckedInputTick = -1;

	std::deque<Input> m_inputs;
	std::deque<std::pair<int, std::unique_ptr<CharacterCore>>> m_history;
	std::deque<Snapshot> m_snapshots;
	float m_lastSnapshotTick = 0.f;
	int m_tick = 0;
	std::vector<std::vector<std::unique_ptr<Entity>>> m_entitiesByType;

	struct Player
	{
		int id = -1;
		EntityType type;
		std::unique_ptr<CharacterCore> m_currentCore;
		std::unique_ptr<CharacterCore> m_prevCore;
	} m_player;

};