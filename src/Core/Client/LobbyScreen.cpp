#include "LobbyScreen.h"
#include "Client.h"
#include "RoomScreen.h"

#include "Core/Logger.h"
#include "Core/Packer.h"
#include "Core/Protocol.h"
#include "Core/ENetUtility.h"


void LobbyScreen::onEnter(Client & client)
{
	loadUi(client);
}

void LobbyScreen::handleEvent(const sf::Event & ev, Client & client)
{
}

void LobbyScreen::handleNetEvent(ENetEvent & netEv, Client & client)
{
	if (netEv.type == ENET_EVENT_TYPE_RECEIVE)
	{
		Unpacker unpacker(netEv.packet->data, netEv.packet->dataLength);
		Msg msg;
		unpacker.unpack(msg);
		if (msg == Msg::SV_ACCEPT_JOIN)
		{
			Logger::getInstance().info("Joined game");
			client.getScreenStack().push(new RoomScreen);
		}
		else if (msg == Msg::SV_REJECT_JOIN)
		{
			std::cout << "rejected from joining game\n";
			client.getNetwork().disconnect();
		}
	}
	else if (netEv.type == ENET_EVENT_TYPE_CONNECT)
	{
		Logger::getInstance().info("Connected to game server");
		Packer packer;
		packer.pack(Msg::CL_REQUEST_JOIN_GAME);
		client.getNetwork().send(packer, true);
		m_connected = true;
	}
	else if (netEv.type == ENET_EVENT_TYPE_DISCONNECT)
	{
		if (m_connected)
		{
			Logger::getInstance().info("Disconnected from game server");
			m_connected = false;
		}
		else
			Logger::getInstance().info("Failed to connecet to game server");

	}

}

void LobbyScreen::handlePacket(Unpacker & unpacker, const ENetAddress & addr, Client & client)
{

}

void LobbyScreen::update(Client & client)
{
}

void LobbyScreen::render(Client & client)
{
}


void LobbyScreen::onExit(Client & client)
{
	hideUi(client);
}

void LobbyScreen::onObscure(Client & client)
{
	hideUi(client);
}

void LobbyScreen::onReveal(Client & client)
{
	loadUi(client);
}

void LobbyScreen::loadUi(Client & client)
{
	m_editBox = tgui::EditBox::create();
	client.getGui().add(m_editBox);

}

void LobbyScreen::hideUi(Client & client)
{
	m_editBox->hide();
}