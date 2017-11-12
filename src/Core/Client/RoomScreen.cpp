#include "RoomScreen.h"
#include "Game/Client/PlayingScreen.h"
#include "Core/Protocol.h"
#include "Core/Logger.h"
#include "Core/Client/Client.h"

void RoomScreen::onEnter(Client & client)
{
}

void RoomScreen::handleEvent(const sf::Event & event, Client & client)
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
		Logger::getInstance().info("RoomScreen", "Disconnected from game server");
	}
}

void RoomScreen::handleUdpPacket(Unpacker & unpacker, const ENetAddress & addr, Client & client)
{
}

void RoomScreen::update(Client & client)
{
}

void RoomScreen::render(Client & client)
{
}

void RoomScreen::onExit(Client & client)
{
	std::cout << "room exit!\ns";
}

void RoomScreen::onObscure(Client & client)
{
}

void RoomScreen::onReveal(Client & client)
{
}
