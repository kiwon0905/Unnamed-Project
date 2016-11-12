#pragma once

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Desktop.hpp>

class Client;

class Gui
{
public:
	bool initialize(Client & client);
	void finalize(Client & client);
	void tick(float dt, Client & client);
	void render(Client & client);

	void handleEvent(const sf::Event & event);

	sfg::SFGUI & getGui();
	sfg::Desktop & getDesktop();

private:
	sfg::SFGUI m_gui;
	sfg::Desktop m_desktop;
};