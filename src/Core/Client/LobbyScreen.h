#pragma once

#include "Screen.h"

#include <vector>
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
class LobbyScreen : public Screen
{
public:
	void onEnter(Client & client);
	void handleEvent(const sf::Event & event, Client & client);
	void handleNetEvent(ENetEvent & netEv, Client & client);
	void handlePacket(Unpacker & unpacker, const ENetAddress & addr, Client & client);
	void update(Client & client);
	void render(Client & client);
	void onExit(Client & client);
	void onObscure(Client & client);
	void onReveal(Client & client);

private:
	tgui::VerticalLayout::Ptr m_layout;
	tgui::EditBox::Ptr m_nameBox;
	tgui::EditBox::Ptr m_addressBox;
	tgui::Button::Ptr m_button;
	bool m_connected = false;
};