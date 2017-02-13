#include "RoomScreen.h"
#include "PlayingScreen.h"
#include "Core/Protocol.h"
#include "Core/Logger.h"

void RoomScreen::onEnter(Client & client)
{
}

void RoomScreen::handleEvent(const sf::Event & ev, Client & client)
{
}

void RoomScreen::handleNetEvent(ENetEvent & netEv, Client & client)
{
	if (netEv.type == ENET_EVENT_TYPE_RECEIVE)
	{
		Unpacker unpacker(netEv.packet->data, netEv.packet->dataLength);
		Msg msg;
		unpacker.unpack(msg);
		if (msg == Msg::SV_LOAD_GAME)
			client.getScreenStack().push(new PlayingScreen);
	}
	else if (netEv.type == ENET_EVENT_TYPE_CONNECT)
	{

	}
	else if (netEv.type == ENET_EVENT_TYPE_DISCONNECT)
	{
		Logger::getInstance().info("Disconnected from game server");
	}
}

void RoomScreen::handlePacket(Unpacker & unpacker, const ENetAddress & addr, Client & client)
{
}

void RoomScreen::update(float dt, Client & client)
{
}

void RoomScreen::render(float t, Client & client)
{
}

void RoomScreen::onExit(Client & client)
{
}

void RoomScreen::onObscure(Client & client)
{
}

void RoomScreen::onReveal(Client & client)
{
}
