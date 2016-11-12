#include "PlayingScreen.h"
#include "Core/Client/Client.h"

void PlayingScreen::onEnter(Client & client)
{
	Packet packet;
	packet << Msg::CL_READY;
	client.getNetwork()->send(packet, true);
	std::cout << "Ready!\n";
}

void PlayingScreen::handleEvent(const sf::Event & ev, Client & client)
{
}

void PlayingScreen::handleNetEvent(NetEvent & netEv, Client & client)
{
	if (netEv.type == NetEvent::Received)
	{
		m_gameWorld.handlePacket(*netEv.packet);
	}
	else if (netEv.type == NetEvent::Connected)
	{

	}
	else if (netEv.type == NetEvent::Disconnected)
	{
		std::cout << "disconnected\n";
	}
	else if (netEv.type == NetEvent::TimedOut)
	{
		std::cout << "timed out\n";
	}
}

void PlayingScreen::handlePacket(Packet & packet, const ENetAddress & addr, Client & client)
{

}

void PlayingScreen::update(float dt, Client & client)
{
	m_gameWorld.update(dt, client);
}

void PlayingScreen::render(Client & client)
{
	m_gameWorld.render(client);
}

void PlayingScreen::onExit(Client & client)
{
}

void PlayingScreen::onObscure(Client & client)
{
}

void PlayingScreen::onReveal(Client & client)
{
}
