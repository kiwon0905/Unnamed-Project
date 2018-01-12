#pragma once

#include "Hud.h"

#include "Game/GameCore.h"
#include "Game/Snapshot.h"
#include "Game/NetInput.h"

#include "Game/Client/Entity.h"
#include "Game/Client/PredictedEntity.h"
#include "Game/Client/RenderMap.h"
#include "Core/Client/Screen.h"
#include "Core/Client/Graph.h"
#include "Core/Client/Particles.h"

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
	friend class Hud;
	struct PlayerInfo
	{
		std::string name;
		int id;

		int entityId;
		Team team;
		int score;
		int kills;
		int deaths;
		int assists;
		int ping;
		int respawnTick;
	};

	PlayingScreen();
	~PlayingScreen() = default;
	void onEnter(Client & client);
	void handleEvent(const sf::Event & event, Client & client);
	void handleNetEvent(ENetEvent & netEv, Client & client);
	void handleUdpPacket(Unpacker & unpacker, const ENetAddress & addr, Client & client);
	void update(Client & client);
	void render(Client & client);
	void onExit(Client & client);
	void onObscure(Client & client);
	void onReveal(Client & client);
private:
	enum State
	{
		LOADING,	//load assets
		ENTERING,	//receive enough snapshots to interpolate, adjust timing
		IN_GAME,
		POST_GAME
	};
	State m_state = LOADING;


	struct PlayerJoinLeave
	{
		int tick;
		int id;
		bool joinOrLeave;//true if joined false if left
	};
	std::deque<PlayerJoinLeave> m_playerJoinOrLeaves;

	int m_myPlayerId;
	std::vector<PlayerInfo> m_players;

	struct Input
	{
		sf::Time predictedTime;
		sf::Clock elapsed;
		NetInput input;
		int tick;
	};
	Input m_inputs[200];
	std::size_t m_currentInputIndex = 0;

	//timing variables
	sf::Clock m_regularClock;
	
	SmoothClock m_renderClock;
	sf::Time m_prevRenderTime;
	sf::Time m_currentRenderTime;
	float m_renderInterTick;

	SmoothClock m_predictedClock;
	sf::Time m_prevPredictedTime;
	sf::Time m_currentPredictedTime;
	sf::Time m_accumulator;
	float m_predictedInterTick;

	//snap variables
	SnapshotContainer m_snapshots;
	int m_numReceivedSnapshots = 0;
	int m_startTick = -1;
	int m_lastRecvTick = -1;
	int m_predictedTick = -1;
	bool m_repredict = false;

	struct SnapInfo
	{
		int tick = -1;
		Snapshot * snapshot = nullptr;
	};
	SnapInfo m_currentSnap;
	SnapInfo m_nextSnap;

	//dbg graph
	std::unique_ptr<Graph> m_predictionGraph;
	std::unique_ptr<Graph> m_snapshotGraph;

	//rendering
	RenderMap m_map;
	sf::View m_view;
	Particles m_particles;
	const sf::Font * m_font;

	//entities
	Entity * getEntity(int id);
	std::vector<std::vector<std::unique_ptr<Entity>>> m_entitiesByType;
	std::vector<PredictedEntity *> m_predictedEntities;

	void debugRender(Client & client, const sf::View & playerView);


	//UI
	tgui::ChatBox::Ptr m_chatBox;
	tgui::EditBox::Ptr m_editBox;
	tgui::Panel::Ptr m_scoreBoard;

	std::unique_ptr<Hud> m_hud;

	void loadUi(Client & client);
	void updateScoreboard();
public:
	Map & getMap();
	PlayerInfo * getPlayerInfo(int id);

	sf::Time getPrevRenderTime() const;
	sf::Time getCurrentRenderTime() const;
	float getRenderInterTick() const;

	sf::Time getPrevPredictedTime() const;
	sf::Time getCurrentPredictedTime() const;
	float getPredictedInterTick() const;

	const SnapInfo & getCurrentSnap() const;
	const SnapInfo & getNextSnap() const;

	const PlayerInfo * getPlayerInfoByEntity(int id);
	const std::vector<PlayerInfo> & getPlayerInfos();
	int getMyPlayerId();

	Particles & getParticles();
	const sf::Font * getFont();

};