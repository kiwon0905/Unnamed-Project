#pragma once

#include "Screen.h"
#include "JobExecutor.h"
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFGUI/Widgets.hpp>

class LobbyScreen : public Screen
{
public:
	void onEnter(Client & client);
	void handleEvent(const sf::Event & ev, Client & client);
	void handleNetEvent(NetEvent & netEv, Client & client);
	void handlePacket(Packet & packet, const ENetAddress & addr, Client & client);
	void update(float dt, Client & client);
	void render(Client & client);
	void onExit(Client & client);
	void onObscure(Client & client);
	void onReveal(Client & client);

private:
	void loadUi(Client & client);
	void hideUi(Client & client);
	void refresh(Client & client);
	void turnPage(int page, Client & client);
private:
	struct ServerInfo
	{
		ENetAddress address;
		sf::Uint32 id;
		std::string name;
	};
	JobExecutor m_jobScheduler;
	std::vector<ServerInfo> m_internetGameServers;
	std::vector<ServerInfo> m_lanGameServers;
	sfg::Window::Ptr m_window;
	sfg::Notebook::Ptr m_notebook;
	sfg::Table::Ptr m_internetTable;
	sfg::Table::Ptr m_lanTable;
	int m_currentPage = 1;
	bool m_connecting = false;
};