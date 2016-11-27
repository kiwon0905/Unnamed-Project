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
		unsigned seq;
		unsigned inputSeq;
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
		unsigned seq;
		unsigned bits;
	};
	
	void update(float dt, Client & client);
	void render(Client & client);
	void handlePacket(Unpacker & unpacker, Client & client);
	
	const std::deque<Snapshot> & getSnapshots();
	const std::deque<Input> & getInputs();
private:
	void interpolate();
	void processEntities(int prev, int current);	//create or destroy new entities
	void processDelta();
	void predict();
	Entity * getEntity(unsigned id);
	Human * createHuman(unsigned id);

	std::deque<Input> m_inputs;
	std::deque<Snapshot> m_snapshots;
	std::vector<std::unique_ptr<Entity>> m_entities;
	bool m_ready = false;
	unsigned m_playerEntityId = 0;
	unsigned m_inputSeq = 1;
	float m_time;
	int m_prevS0 = -1;
};