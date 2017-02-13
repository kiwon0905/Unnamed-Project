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
		int seq;
		unsigned bits;
	};
	
	GameWorld();

	void onDisconnect();
	void update(float dt, Client & client);
	void render(float t, Client & client);

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
	bool m_loaded = false;
	float m_delay = .1f;

	int m_nextInputSeq = 0;
	int m_lastAckedInputSeq = -1;

	std::deque<Input> m_inputs;
	std::deque<std::pair<int, std::unique_ptr<CharacterCore>>> m_history;
	std::deque<Snapshot> m_snapshots;
	int m_tickSinceLastSnapshot = 0;
	std::vector<std::vector<std::unique_ptr<Entity>>> m_entitiesByType;
	int m_playerEntityId = -1;
	EntityType m_playerEntityType = EntityType::NONE;
	std::unique_ptr<CharacterCore> m_playerCore;
	sf::Vector2f m_oldPos;
};