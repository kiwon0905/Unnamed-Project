#include "RoomScreen.h"
#include "Game/Client/PlayingScreen.h"
#include "Core/Protocol.h"
#include "Core/Logger.h"
#include "Core/Client/Client.h"

void RoomScreen::onEnter(Client & client)
{
	Packer packer;
	packer.pack(Msg::CL_REQUEST_ROOM_INFO);
	client.getNetwork().send(packer, true);
	Packer packer2;
	packer2.pack(Msg::CL_REQUEST_PLAYER_INFO);
	client.getNetwork().send(packer2, true);

	m_editBox = tgui::EditBox::create();
	m_editBox->setPosition({ 0.f, client.getWindow().getSize().y - m_editBox->getSize().y });
	m_editBox->setSize("50%", m_editBox->getSize().y);

	auto sendChat = [&client, this]()
	{
		std::string chat = m_editBox->getText().toAnsiString();
		if (chat != "")
		{
			Packer packer3;
			packer3.pack(Msg::CL_CHAT);
			packer3.pack(m_editBox->getText().toAnsiString());
			client.getNetwork().send(packer3, true);
			m_editBox->setText("");
		}
	};
	m_editBox->onReturnKeyPress.connect(sendChat);

	client.getGui().add(m_editBox);
	m_chatBox = tgui::ChatBox::create();
	m_chatBox->setPosition({ 0.f, m_editBox->getPosition().y - m_chatBox->getSize().y });
	m_chatBox->setSize("50%", m_chatBox->getSize().y);
	client.getGui().add(m_chatBox);

	m_backButton = tgui::Button::create("Back");

	auto onClick = [&client]()
	{
		client.getNetwork().disconnect();
		client.getScreenStack().pop();
	};
	m_backButton->onClick.connect(onClick);
	client.getGui().add(m_backButton);
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
		if (msg == Msg::SV_PLAYER_INFO)
		{
			std::size_t count;
			unpacker.unpack(count);
			int myId;
			unpacker.unpack(myId);
			for (std::size_t i = 0; i < count; ++i)
			{
				Player p;
				unpacker.unpack(p.id);
				unpacker.unpack(p.name);
				std::string line = p.name + " has joined.";
				m_chatBox->addLine(line, sf::Color::Red);
				m_players.push_back(p);		
			}
		}
		else if (msg == Msg::SV_PLAYER_JOINED)
		{
			Player p;
			unpacker.unpack(p.id);
			unpacker.unpack(p.name);
			std::string line = p.name + " has joined.";
			m_chatBox->addLine(line, sf::Color::Red);
			m_players.push_back(p);
		}
		else if (msg == Msg::SV_PLAYER_LEFT)
		{
			int id;
			unpacker.unpack(id);
			for (auto & p : m_players)
			{
				if (p.id == id)
				{
					std::string line = p.name + " has left.";
					Logger::getInstance().info("Chat", line);
					m_chatBox->addLine(line);
					break;
				}
			}
		}
		else if (msg == Msg::SV_CHAT)
		{
			int tick;
			int id;
			std::string msg;
			unpacker.unpack(tick);
			unpacker.unpack(id);
			unpacker.unpack(msg);
			Player * p = nullptr;
			for (auto & p : m_players)
			{
				if (p.id == id)
				{
					std::string line = p.name + ": " + msg;
					Logger::getInstance().info("Chat", line);
					m_chatBox->addLine(line);					
					break;
				}
			}
				
		}
		else if (msg == Msg::SV_LOAD_GAME)
		{
			client.getScreenStack().push(new PlayingScreen);
		}
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
	client.getGui().remove(m_editBox);
	client.getGui().remove(m_chatBox);
	client.getGui().remove(m_backButton);
}

void RoomScreen::onObscure(Client & client)
{
	m_backButton->hide();
	m_chatBox->hide();
	m_editBox->hide();
}

void RoomScreen::onReveal(Client & client)
{
	m_backButton->show();
	m_chatBox->show();
	m_editBox->show();
}
