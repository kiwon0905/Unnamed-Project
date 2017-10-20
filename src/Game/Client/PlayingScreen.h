#pragma once

#include "Core/Client/Screen.h"
#include "Core/Client/Graph.h"
#include "Core/Client/Particles.h"
#include "Game/GameCore.h"
#include "Game/Snapshot.h"
#include "Game/Client/Entity.h"
#include "Game/Client/PredictedEntity.h"
#include "Game/NetInput.h"
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

class Announcer : public sf::Drawable
{
public:
	void setFont(const sf::Font & font);
	void announce(const std::string & s, const sf::Color & fill = sf::Color::White, const sf::Color & outline = sf::Color::Black);
	void update(float dt);
	void draw(sf::RenderTarget & target, sf::RenderStates states) const;
private:
	const sf::Font * m_font;
	struct Info
	{
		sf::Text text;
		float time = 0.f;
	};
	std::deque<Info> m_infos;
};

class PlayingScreen : public Screen
{
public:
	PlayingScreen();
	~PlayingScreen() { std::cout << "play destroy!"; }
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


	struct PlayerInfo
	{
		int id;
		int entityId;
		Team team;
		std::string name;
	};
	std::vector<PlayerInfo> m_players;
	PlayerInfo m_myPlayer;

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
	sf::Time m_prevPredictedTime;
	SmoothClock m_renderTime;
	SmoothClock m_predictedTime;
	sf::Time m_accumulator;
	SnapshotContainer m_snapshots;
	int m_numReceivedSnapshots = 0;
	int m_startTick = -1;
	int m_lastRecvTick = -1;
	int m_predictedTick = -1;
	int m_prevRenderTick = -1;
	int m_currentRenderTick = -1;
	bool m_repredict = false;

	std::unique_ptr<Graph> m_predictionGraph;
	std::unique_ptr<Graph> m_snapshotGraph;


	//rendering
	Map m_map;
	sf::Texture * m_tileTexture;
	sf::RenderTexture m_renderTexture;
	sf::VertexArray m_tileVertices;
	sf::View m_view;
	Particles m_particles;
	Announcer m_announcer;
	//entities
	Entity * getEntity(int id);
	std::vector<std::vector<std::unique_ptr<Entity>>> m_entitiesByType;
	std::vector<PredictedEntity *> m_predictedEntities;

	void debugRender(Client & client, const sf::View & playerView);


	//UI
	tgui::ChatBox::Ptr m_chatBox;
	tgui::EditBox::Ptr m_editBox;
	tgui::Panel::Ptr m_scoreBoard;

public:
	const PlayerInfo * getPlayerInfo(int id);
	const PlayerInfo * getPlayerInfoByEntityId(int entityId);
	const PlayerInfo & getMyPlayerInfo();
	Particles & getParticles();

};