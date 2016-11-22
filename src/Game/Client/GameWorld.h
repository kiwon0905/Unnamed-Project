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
		struct EntityInfo
		{
			EntityType type;
			float x;
			float y;
		};
		std::unordered_map<unsigned, EntityInfo> entityInfo;
	};

	void update(float dt, Client & client);
	void render(Client & client);
	void handlePacket(Unpacker & unpacker, Client & client);
	const std::deque<Snapshot> & getSnapshots();
	
	unsigned getInputSeq();
	unsigned getLastInput();
	unsigned getAckedInputSeq();
private:

	void processDelta();

	Entity * getEntity(unsigned id);
	Human * createHuman(unsigned id);

	std::deque<Snapshot> m_snapshots;
	std::vector<std::unique_ptr<Entity>> m_entities;
	unsigned m_playerEntityId = 0;
	unsigned m_inputSeq = 1;
	unsigned m_ackedInputSeq = 0;
	unsigned m_lastInput;
};