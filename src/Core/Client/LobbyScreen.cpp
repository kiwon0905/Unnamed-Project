#include "LobbyScreen.h"
#include "Client.h"
#include "RoomScreen.h"

#include "Core/Logger.h"
#include "Core/Packer.h"
#include "Core/Protocol.h"
#include "Core/ENetUtility.h"

template <typename T>
void setCenter(T & widget)
{
	
}
void LobbyScreen::onEnter(Client & client)
{
	auto & window = client.getWindow();

	auto & gui = client.getGui();
	auto connect = [this, &client]()
	{
		std::string name = m_nameBox->getText();
		std::cout << "name is: " << name << "\n";

		std::string text = m_addressBox->getText();
		ENetAddress addr;
		if (enutil::toENetAddress(text, addr))
		{
			client.getNetwork().connect(addr);
		}
		else
		{
			std::cout << "invalid address\n";
		}
	};

	m_nameBox = tgui::EditBox::create();
	m_nameBox->setDefaultText("Nickname");

	m_addressBox = tgui::EditBox::create();
	m_addressBox->setDefaultText("Server address");
	
	
	m_button = tgui::Button::create("connect");
	m_button->setPosition({ 0, 100 });
	m_button->onPress->connect(connect);

	m_layout = tgui::VerticalLayout::create();
	m_layout->setSize({ "30%", "20%" });
	m_layout->setPosition("35%", "50%");
	setCenter(m_layout);
	m_layout->add(m_nameBox);
	m_layout->add(m_addressBox);
	m_layout->add(m_button);

	gui.add(m_layout);
	m_layout->getRenderer()->setSpaceBetweenWidgets(5.f);
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
		packer.pack(m_nameBox->getText().toAnsiString());
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

	client.getGui().remove(m_layout);
}

void LobbyScreen::onObscure(Client & client)
{
	m_layout->hide();

}

void LobbyScreen::onReveal(Client & client)
{
	m_layout->show();
}