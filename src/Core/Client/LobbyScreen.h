#pragma once

#include "Screen.h"

#include <vector>
#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>


#include <SFML/Audio.hpp>
class LobbyScreen : public Screen
{
public:
	LobbyScreen();
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
	void requestInternetGamesInfo(Client & client);
	void updateInternetGamesUi(Client & client);
	void loadPrevMusic();
	void loadNextMusic();
	void checkPing(Client & client);

	struct GameInfo
	{
		enum Status
		{
			PRE_GAME,
			LOADING,
			IN_GAME,
			POST_GAME,
			COUNT
		};
		int id;
		ENetAddress addr;
		enet_uint16 pingCheckPort;
		std::string name;
		std::string modeName;
		Status status;
		int numPlayers;
		sf::Clock lastPingReq;
	};
	enum
	{
		INTERNET = 0,
		LAN,
		PRIVATE,
		SETTINGS,
		COUNT
	};

	std::vector<GameInfo> m_internetGames;

	sf::Sprite m_background;
	sf::FloatRect m_backgroundTextureRect;
	sf::Clock m_clock;

	sf::Music m_music;
	std::vector<std::string> m_musics;
	std::size_t m_currentMusicIndex = -1;
	sf::Music::Status m_prevStatus = sf::Music::Stopped;

	tgui::Tabs::Ptr m_tabs;
	std::vector<tgui::Panel::Ptr> m_panels;
	tgui::Panel::Ptr m_musicPanel;
	
	tgui::Texture m_prevTexture;
	tgui::Texture m_pauseTexture;
	tgui::Texture m_playTexture;
	tgui::Texture m_nextTexture;

	std::unique_ptr<std::thread> m_fadeOutThread;
};