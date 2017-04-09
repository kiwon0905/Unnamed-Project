#pragma once

#include "Screen.h"
#include "Game/CharacterCore.h"
#include "Game/Snapshot.h"
#include "Game/Client/Entity.h"
#include <SFML/Graphics.hpp>


class SmoothClock
{
public:
	sf::Time getElapsedTime();
	void reset(sf::Time target = sf::Time::Zero);
	void update(sf::Time target, sf::Time converge);
private:
	sf::Clock m_clock;
	sf::Time m_snap;
	sf::Time m_current;
	sf::Time m_target;
	sf::Time m_converge;
};

class PlayingScreen : public Screen
{
public:
	PlayingScreen();
	~PlayingScreen() { std::cout << "play destroy!"; }
	virtual void onEnter(Client & client);
	virtual void handleEvent(const sf::Event & ev, Client & client);
	virtual void handleNetEvent(ENetEvent & netEv, Client & client);
	virtual void handlePacket(Unpacker & unpacker, const ENetAddress & addr, Client & client);
	virtual void update(Client & client);
	virtual void render(Client & client);
	virtual void onExit(Client & client);
	virtual void onObscure(Client & client);
	virtual void onReveal(Client & client);
private:
	enum State
	{
		LOADING,	//load assets
		ENTERING,	//receive enough snapshots to interpolate, adjust timing
		IN_GAME
	};
	struct PlayerInfo
	{
		int id;
		int entityId;
	};
	std::vector<PlayerInfo> m_players;
	PlayerInfo m_myPlayer;
	State m_state = LOADING;

	struct Input
	{
		unsigned data;
		int tick;
		sf::Time predictedTime;
		sf::Clock elapsed;
	};
	Input m_inputs[200];
	std::size_t m_currentInputIndex = 0;

	sf::Time m_prevPredictedTime;
	int m_renderDelayTick = 4;
	SmoothClock m_renderTime;
	SmoothClock m_predictedTime;
	SnapshotContainer m_snapshots;
	int m_numReceivedSnapshots = 0;
	int m_startTick = -1;
	int m_lastSnapshotTick = -1;
	int m_predictedTick = -1;
	bool m_repredict = false;

	std::unique_ptr<CharacterCore> m_playerCurrentCore;
	std::unique_ptr<CharacterCore> m_playerPrevCore;

	std::vector<std::vector<std::unique_ptr<Entity>>> m_entitiesByType;

	Map m_map;
	sf::Texture * m_tileTexture;
	sf::RenderTexture m_renderTexture;
	sf::VertexArray m_tileVertices;
	sf::Vector2f m_cameraSize;

	Entity * getEntity(int id);
};