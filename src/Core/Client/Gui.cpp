#include "Gui.h"
#include "Client.h"

bool Gui::initialize(Client & client)
{
	return true;
}

void Gui::finalize(Client & client)
{
}

void Gui::update(float dt, Client & client)
{
	m_desktop.Update(dt);
}

void Gui::render(Client & client)
{
	m_gui.Display(client.getContext().window);
}

void Gui::handleEvent(const sf::Event & event)
{
	m_desktop.HandleEvent(event);
}

sfg::SFGUI & Gui::getGui()
{
	return m_gui;
}

sfg::Desktop & Gui::getDesktop()
{
	return m_desktop;
}
