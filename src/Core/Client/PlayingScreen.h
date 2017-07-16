#pragma once

#include "Screen.h"
#include "Core/Client/Graph.h"
#include "Core/Client/Particles.h"
#include "Game/GameCore.h"
#include "Game/Snapshot.h"
#include "Game/Client/Entity.h"
#include "Game/NetInput.h"
#include <SFML/Graphics.hpp>


class SmoothClock
{
public:
	sf::Time getElapsedTime();
	void reset(sf::Time target = sf::Time::Zero);
	void update(sf::Time target, sf::Time converge);
	void update2(sf::Time target, sf::Time timeLeft, int adjustDirection);
	void setAdjustSpeed(int direction, float speed);
private:
	sf::Clock m_clock;
	sf::Time m_snap;
	sf::Time m_current;
	sf::Time m_target;
	sf::Time m_converge;

	float m_adjustSpeed[2];
	int m_spikeCounter;
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
		IN_GAME,
		POST_GAME
	};
	struct PlayerInfo
	{
		int id;
		int entityId;
		Team team;
	};
	std::vector<PlayerInfo> m_players;
	PlayerInfo m_myPlayer;
	State m_state = LOADING;

	struct Input
	{
		sf::Time predictedTime;
		sf::Clock elapsed;
		NetInput input;
	};
	Input m_inputs[200];
	std::size_t m_currentInputIndex = 0;

	//timing variables
	sf::Time m_prevPredictedTime;
	SmoothClock m_renderTime;
	SmoothClock m_predictedTime;
	sf::Time m_accumulator;
	SnapshotContainer m_snapshots;
	int m_numReceivedSnapshots = 0;
	int m_startTick = -1;
	int m_lastSnapshotTick = -1;
	int m_predictedTick = -1;
	bool m_repredict = false;

	std::unique_ptr<Graph> m_predictionGraph;
	std::unique_ptr<Graph> m_snapshotGraph;


	//rendering
	Map m_map;
	sf::Texture * m_tileTexture;
	sf::RenderTexture m_renderTexture;
	sf::VertexArray m_tileVertices;
	sf::View m_view;
	bool m_debugRender = false;

	struct Particles
	{
		std::unique_ptr<SmokeParticles> smoke;
	} m_particles;
	//entities
	Entity * getEntity(int id);
	std::vector<std::vector<std::unique_ptr<Entity>>> m_entitiesByType;

public:
	const PlayerInfo * getPlayerInfo(int entityId);
	const PlayerInfo & getMyPlayerInfo();
	Particles & getParticles();

};