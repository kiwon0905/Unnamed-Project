#pragma once
#include "Human.h"
#include "Game/GameCore.h"
#include "Game/CharacterCore.h"
#include "Core/Client/Client.h"
#include "Core/Packer.h"
#include "Game/NetObject.h"
#include "Game/Map.h"

#include <unordered_map>
#include <deque>

class GameWorld
{
public:
	struct Snapshot
	{
		int tick;
		float clientTick;
		std::unordered_map<int, std::unique_ptr<NetEntity>> entities;
	};
	struct Input
	{
		int tick;
		unsigned bits;
	};
	
	GameWorld();

	void init(Client & client);
	void onDisconnect();
	void update(float dt, Client & client);
	void render(Client & client);

	void onWorldInfo(Unpacker & unpacker, Client & client);
	void onSnapshot(Unpacker & unpacker, Client & client);
private:
	enum State
	{
		LOADING,
		ENTERING,
		IN_GAME
	};
	Entity * createEntity(int id, EntityType type);
	Entity * getEntity(int id, EntityType type);
	CharacterCore * createCharacterCore(EntityType type);

	State m_state = LOADING;
	float m_delay = .1f;

	bool m_repredict = false;
	int m_lastAckedInputTick = -1;
	int m_tick = 0;

	std::deque<std::unique_ptr<Snapshot>> m_snapshots;
	Snapshot * m_prevSnapshot = nullptr;

	std::string m_mapName;
	Map m_map;
	sf::VertexArray m_tileVertices;
	sf::Texture * m_tileTexture;
	std::vector<std::vector<std::unique_ptr<Entity>>> m_entitiesByType;

	struct Player
	{
		int id = -1;
		EntityType type;
		std::unique_ptr<CharacterCore> currentCore;
		std::unique_ptr<CharacterCore> prevCore;
		std::deque<std::pair<int, std::unique_ptr<CharacterCore>>> history;
		std::deque<Input> inputs;
	} m_player;

	sf::Vector2f m_cameraSize;
	sf::RenderTexture m_renderTexture;
};