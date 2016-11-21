#include "PlayingScreen.h"
#include "Core/Client/Client.h"
#include "Core/Packer.h"
#include "Core/Protocol.h"

void PlayingScreen::onEnter(Client & client)
{
	Packer packer;
	packer.pack(Msg::CL_READY);
	client.getNetwork().send(packer, true);
}

void PlayingScreen::handleEvent(const sf::Event & ev, Client & client)
{
}

void PlayingScreen::handleNetEvent(NetEvent & netEv, Client & client)
{
	if (netEv.type == NetEvent::RECEIVED)
	{
		Unpacker unpacker(netEv.packet->data, netEv.packet->dataLength);
		m_gameWorld.handlePacket(unpacker);
	}
	else if (netEv.type == NetEvent::CONNECTED)
	{

	}
	else if (netEv.type == NetEvent::DISCONNECTED)
	{
		std::cout << "disconnected\n";
	}
	else if (netEv.type == NetEvent::TIMED_OUT)
	{
		std::cout << "timed out\n";
	}
}

void PlayingScreen::handlePacket(Unpacker & unpacker, const ENetAddress & addr, Client & client)
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
