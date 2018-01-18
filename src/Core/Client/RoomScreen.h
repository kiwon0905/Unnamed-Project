#pragma once

#include "Screen.h"
#include <TGUI/TGUI.hpp>
#include <vector>

class RoomScreen : public Screen
{
public:
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
	struct Player
	{
		int id;
		std::string name;
	};
	tgui::Button::Ptr m_backButton;
	tgui::ChatBox::Ptr m_chatBox;
	tgui::EditBox::Ptr m_editBox;
	std::vector<Player> m_players;

};