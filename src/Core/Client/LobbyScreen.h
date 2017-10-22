#pragma once

#include "Screen.h"

#include <vector>
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
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
		std::string name;
		std::string modeName;
		Status status;
		int numPlayers;
	};
	std::vector<GameInfo> m_internetGames;

	tgui::Tabs::Ptr m_tabs;
	enum
	{
		INTERNET = 0,
		LAN,
		PRIVATE,
		SETTINGS,
		COUNT
	};
	sf::Sprite m_background;
	sf::FloatRect m_backgroundTextureRect;
	sf::Clock m_clock;
	std::vector<tgui::Panel::Ptr> m_panels;
	bool m_connected = false;
};