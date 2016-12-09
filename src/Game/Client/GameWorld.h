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
		int seq;
		int inputSeq;
		float time;
		struct EntityInfo
		{
			EntityType type;
			sf::Vector2f pos;
		};
		std::unordered_map<unsigned, EntityInfo> entityInfo;
	};
	struct Input
	{
		int seq;
		unsigned bits;
	};
	
	void onDisconnect();
	void update(float dt, Client & client);
	void render(Client & client);
	void handlePacket(Unpacker & unpacker, Client & client);
	
	const std::deque<Snapshot> & getSnapshots();
	const std::deque<Input> & getInputs();
private:
	void interpolate(float delay);
	void processEntities(int prev, int current);	//create or destroy new entities
	void processDelta();
	void predict();
	Entity * getEntity(unsigned id);
	Human * createHuman(unsigned id);

	std::deque<Input> m_inputs;
	std::deque<Snapshot> m_snapshots;
	std::vector<std::unique_ptr<Entity>> m_entities;
	bool m_ready = false;
	float m_time = 0.f;
	int m_playerEntityId = -1;
	int m_inputSeq = 0;
	int m_prevS0 = -1;
	int m_lastSnapshotSeq = -1;
};