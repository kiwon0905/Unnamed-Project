#include "PlayingScreen.h"
#include "Core/Client/Client.h"
#include "Core/Packer.h"
#include "Core/Protocol.h"

void PlayingScreen::onEnter(Client & client)
{
	Packer packer;
	packer.pack(Msg::CL_REQUEST_WORLD_INFO);

	client.getNetwork().send(packer, true);
}

void PlayingScreen::handleEvent(const sf::Event & ev, Client & client)
{

}

void PlayingScreen::handleNetEvent(ENetEvent & netEv, Client & client)
{
	if (netEv.type == ENET_EVENT_TYPE_RECEIVE)
	{
		Unpacker unpacker(netEv.packet->data, netEv.packet->dataLength);
		m_gameWorld.handlePacket(unpacker, client);
	}
	else if (netEv.type == ENET_EVENT_TYPE_CONNECT)
	{

	}
	else if (netEv.type == ENET_EVENT_TYPE_DISCONNECT)
	{
		m_gameWorld.onDisconnect();
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
