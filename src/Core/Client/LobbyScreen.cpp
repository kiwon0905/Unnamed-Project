#include "LobbyScreen.h"
#include "Client.h"
#include "RoomScreen.h"

#include "Core/Logger.h"
#include "Core/Packer.h"
#include "Core/Protocol.h"
#include "Core/ENetUtility.h"


LobbyScreen::LobbyScreen()
{
	m_panels.resize(COUNT);
}

void LobbyScreen::onEnter(Client & client)
{
	auto & window = client.getWindow();
	auto & gui = client.getGui();

	m_tabs = tgui::Tabs::create();
	gui.add(m_tabs);
	m_tabs->setPosition({ "10%", "8.5%" });
	m_tabs->add("Internet", true);
	m_tabs->add("LAN", false);
	m_tabs->add("Private", false);
	m_tabs->add("Settings", false);

	auto onTabSelect = [this](const sf::String & s)
	{
		for (auto & p : m_panels)
			p->hide();
		if (s == "Internet")
		{
			m_panels[INTERNET]->show();
		}
		else if (s == "LAN")
		{
			m_panels[LAN]->show();
		}
		else if (s == "Private")
		{
			m_panels[PRIVATE]->show();
		}
		else if (s == "Settings")
		{
			m_panels[SETTINGS]->show();
		}

	};
	m_tabs->onTabSelect.connect(onTabSelect);


	for (auto & p : m_panels)
	{
		p = tgui::Panel::create({ "80%", "50%" });
		p->setPosition({ tgui::bindLeft(m_tabs), tgui::bindBottom(m_tabs) + 2 });
		p->getRenderer()->setBackgroundColor(sf::Color::White);
		p->hide();
		gui.add(p);
	}
	m_panels[INTERNET]->show();


	auto topPanel = tgui::Panel::create({ "100%", "10%" });
		topPanel->getRenderer()->setBackgroundColor(sf::Color::Green);
		topPanel->getRenderer()->setBorders(2);

		auto name = tgui::Label::create("Name");
		name->setSize("50%", "100%");
		name->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Left);
		name->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
		topPanel->add(name);
	
		auto mode = tgui::Label::create("Mode");
		mode->setPosition({ tgui::bindRight(name), tgui::bindTop(name) });
		mode->setSize({ "12.5%", "100%" });
		mode->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Left);
		mode->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
		topPanel->add(mode);

		auto status = tgui::Label::create("Status");
		status->setPosition({ tgui::bindRight(mode), tgui::bindTop(mode) });
		status->setSize({ "12.5%", "100%" });
		status->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Left);
		status->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
		topPanel->add(status);

		auto players = tgui::Label::create("Players");
		players->setPosition({ tgui::bindRight(status), tgui::bindTop(status) });
		players->setSize({ "12.5%", "100%" });
		players->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Left);
		players->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
		topPanel->add(players);

		auto ping = tgui::Label::create("Ping");
		ping->setSize({ "12.5%", "100%" });
		ping->setPosition({ tgui::bindRight(players), tgui::bindTop(players) });
		ping->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Left);
		ping->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
		topPanel->add(ping);
	m_panels[INTERNET]->add(topPanel);

	
	auto bottomPanel = tgui::Panel::create({ "100%", "90% - 2" });
		bottomPanel->setPosition({ "0%", "10% + 2" });
		bottomPanel->getRenderer()->setBorders(2);
	
		for (std::size_t i = 0; i < 10; ++i)
		{
			auto line = tgui::Panel::create({ "100%", "10%" });
				if (i % 2)
					line->getRenderer()->setBackgroundColor(sf::Color(173, 216, 230));
				else
					line->getRenderer()->setBackgroundColor(sf::Color(176, 224, 230));

				line->setPosition({ "0%", std::to_string(i * 10) + "%" });

				auto name = tgui::Label::create();
				name->setSize("50%", "100%");
				name->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Left);
				name->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
				line->add(name, "name");

				auto mode = tgui::Label::create();
				mode->setPosition({ tgui::bindRight(name), tgui::bindTop(name) });
				mode->setSize({ "12.5%", "100%" });
				mode->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Left);
				mode->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
				line->add(mode, "mode");

				auto status = tgui::Label::create();
				status->setPosition({ tgui::bindRight(mode), tgui::bindTop(mode) });
				status->setSize({ "12.5%", "100%" });
				status->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Left);
				status->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
				line->add(status, "status");

				auto players = tgui::Label::create();
				players->setPosition({ tgui::bindRight(status), tgui::bindTop(status) });
				players->setSize({ "12.5%", "100%" });
				players->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Left);
				players->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
				line->add(players, "players");

				auto ping = tgui::Label::create();
				ping->setSize({ "12.5%", "100%" });
				ping->setPosition({ tgui::bindRight(players), tgui::bindTop(mode) });
				ping->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Left);
				ping->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
				line->add(ping, "ping");
			bottomPanel->add(line);
		}
	m_panels[INTERNET]->add(bottomPanel, "bottomPanel");
	requestInternetGamesInfo(client);
}

void LobbyScreen::handleEvent(const sf::Event & event, Client & client)
{
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R && event.key.control)
	{
		requestInternetGamesInfo(client);

	}
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
			Logger::getInstance().info("LobbyScreen", "Joined game");
			client.getScreenStack().push(new RoomScreen);
		}
		else if (msg == Msg::SV_REJECT_JOIN)
		{
			Logger::getInstance().info("LobbyScreen", "rejected from server");
			client.getNetwork().disconnect();
		}
	}
	else if (netEv.type == ENET_EVENT_TYPE_CONNECT)
	{
		Logger::getInstance().info("LobbyScreen", "Connected to game server");
		Packer packer;
		packer.pack(Msg::CL_REQUEST_JOIN_GAME);
		packer.pack(std::string("Unnamed Person"));
		client.getNetwork().send(packer, true);
		m_connected = true;
	}
	else if (netEv.type == ENET_EVENT_TYPE_DISCONNECT)
	{
		if (m_connected)
		{
			Logger::getInstance().info("LobbyScreen", "Disconnected from game server");
			m_connected = false;
		}
		else
			Logger::getInstance().info("LobbyScreen", "Failed to connecet to game server");

	}

}

void LobbyScreen::handleUdpPacket(Unpacker & unpacker, const ENetAddress & addr, Client & client)
{
	Msg msg;
	unpacker.unpack(msg);
	if (client.getMasterServerAddress() && client.getMasterServerAddress()->host == addr.host && client.getMasterServerAddress()->port == addr.port)
	{
		if (msg == Msg::MSV_INTERNET_SERVER_INFO)
		{
			std::cout << "master server info received\n";
			m_internetGames.clear();
			uint32_t count;
			unpacker.unpack(count);

			for (std::size_t i = 0; i < count; ++i)
			{
				GameInfo info;
				unpacker.unpack(info.addr.host);
				unpacker.unpack(info.addr.port);
				unpacker.unpack(info.id);
				unpacker.unpack(info.name);
				unpacker.unpack(info.modeName);
				unpacker.unpack(info.status);

				std::string addr;
				enutil::toString(info.addr, addr);
				std::cout << info.id << " - " << info.name << ": " << addr << "\n";
				m_internetGames.push_back(info);

			}
			updateInternetGamesUi(client);

		}
	}
}

void LobbyScreen::update(Client & client)
{
}

void LobbyScreen::render(Client & client)
{
}


void LobbyScreen::onExit(Client & client)
{

}

void LobbyScreen::onObscure(Client & client)
{
	m_tabs->hide();
	for (auto & p : m_panels)
		p->hide();
}

void LobbyScreen::onReveal(Client & client)
{
	m_tabs->show();
	for (auto & p : m_panels)
		p->hide();
	std::string s = m_tabs->getSelected();
	if (s == "Internet")
	{
		m_panels[INTERNET]->show();
	}
	else if (s == "LAN")
	{
		m_panels[LAN]->show();
	}
	else if (s == "Private")
	{
		m_panels[PRIVATE]->show();
	}
	else if (s == "Settings")
	{
		m_panels[SETTINGS]->show();
	}

}

void LobbyScreen::requestInternetGamesInfo(Client & client)
{
	const ENetAddress * masterAddr = client.getMasterServerAddress();
	if (masterAddr)
	{
		std::cout << "sent request!\n";
		Packer packer;
		packer.pack(Msg::CL_REQUEST_INTERNET_SERVER_INFO);
		client.getNetwork().send(packer, *masterAddr);
	}
}

void LobbyScreen::updateInternetGamesUi(Client & client)
{
	auto bottomPanel = m_panels[INTERNET]->get<tgui::Panel>("bottomPanel");
	std::size_t i = 0;
	for (auto & linePanel : bottomPanel->getWidgets())
	{
		auto l = std::static_pointer_cast<tgui::Panel>(linePanel);
		auto name = std::static_pointer_cast<tgui::Label>(l->get("name"));
		auto mode = std::static_pointer_cast<tgui::Label>(l->get("mode"));
		auto status = std::static_pointer_cast<tgui::Label>(l->get("status"));
		auto players = std::static_pointer_cast<tgui::Label>(l->get("players"));
		auto ping = std::static_pointer_cast<tgui::Label>(l->get("ping"));

		name->setText("");
		mode->setText("");
		status->setText("");
		players->setText("");
		ping->setText("");
		l->onClick.disconnectAll();
		if (i < m_internetGames.size())
		{
			const GameInfo & info = m_internetGames[i];

			name->setText(info.name);
			mode->setText(info.modeName);
			status->setText(info.status == GameInfo::WAITING ? "Waiting" : "In Game");
			players->setText("0/0");
			ping->setText("0");

			auto connectFunc = [&info, &client]()
			{
				std::cout << "connecting to: " << info.id << " - " << info.name << "\n";
				client.getNetwork().connect(info.addr);
			};
			l->onClick.connect(connectFunc);

		}
		++i;
	}
}
