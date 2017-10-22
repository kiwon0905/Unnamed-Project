#include "LobbyScreen.h"
#include "Client.h"
#include "RoomScreen.h"

#include "Core/Logger.h"
#include "Core/Packer.h"
#include "Core/Protocol.h"
#include "Core/ENetUtility.h"

#include <filesystem>


LobbyScreen::LobbyScreen()
{
	m_panels.resize(COUNT);
}

void LobbyScreen::onEnter(Client & client)
{
	requestInternetGamesInfo(client);

	auto & window = client.getWindow();
	auto & gui = client.getGui();

	sf::Texture * backgroundTexture = client.getAssetManager().get<sf::Texture>("assets/colored_talltrees.png");
	backgroundTexture->setRepeated(true);

	m_background.setTexture(*backgroundTexture);
	m_backgroundTextureRect.width = static_cast<float>(client.getWindow().getSize().x);
	m_backgroundTextureRect.height = static_cast<float>(client.getWindow().getSize().y);


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

	
	auto bottomPanel = tgui::ScrollablePanel::create({ "100%", "90% - 2" });
		bottomPanel->setPosition({ "0%", "10% + 2" });
		bottomPanel->getRenderer()->setBorders(2);
	
		auto grid = tgui::Grid::create();
		bottomPanel->add(grid, "grid");
		
	m_panels[INTERNET]->add(bottomPanel, "bottomPanel");

	m_musicPanel = tgui::Panel::create({ "20%", "10%" });
	m_musicPanel->setPosition({ "40%", "70%" });
	auto horizontalLayout = tgui::HorizontalLayout::create();
	m_musicPanel->add(horizontalLayout);

	m_prevTexture.load("assets/prev.png", {}, {}, true);
	m_pauseTexture.load("assets/pause.png", {}, {}, true);
	m_playTexture.load("assets/play.png", {}, {}, true);
	m_nextTexture.load("assets/next.png", {}, {}, true);

	
	auto prev = tgui::Picture::create(m_prevTexture);
	auto play = tgui::Picture::create(m_playTexture);
	auto pause = tgui::Picture::create(m_pauseTexture);
	auto next = tgui::Picture::create(m_nextTexture);

	auto onPrevClick = [this]()
	{
		loadPrevMusic();
	};
	prev->onClick.connect(onPrevClick);


	auto onPlayClick = [this, horizontalLayout, prev, pause, next]()
	{
		std::cout << "play\n";
		horizontalLayout->removeAllWidgets();
		horizontalLayout->add(prev);
		horizontalLayout->addSpace(.3f);
		horizontalLayout->add(pause, "pause");
		horizontalLayout->addSpace(.3f);
		horizontalLayout->add(next);
		m_music.play();

	};
	play->onClick.connect(onPlayClick);


	auto onPauseClick = [this, horizontalLayout, prev, play, next]()
	{
		std::cout << "pause\n";
		horizontalLayout->removeAllWidgets();
		horizontalLayout->add(prev);
		horizontalLayout->addSpace(.3f);
		horizontalLayout->add(play, "play");
		horizontalLayout->addSpace(.3f);
		horizontalLayout->add(next);
		m_music.pause();
		
	};
	pause->onClick.connect(onPauseClick);


	auto onNextClick = [this]()
	{
		loadNextMusic();
	};
	next->onClick.connect(onNextClick);
	
	horizontalLayout->add(prev);
	horizontalLayout->addSpace(.3f);
	horizontalLayout->add(pause, "pause");
	horizontalLayout->addSpace(.3f);
	horizontalLayout->add(next);

	gui.add(m_musicPanel);


	//music
	std::unordered_set<std::string> supportedFormats = { "ogg", "wav", "flac", "aiff", "au", "raw",
		"paf", "svx", "nist", "voc", "ircam", "w64", "mat4", "mat5", "pvf", "htk", "sds", "avr", "sd2", "caf", "wve", "mpc2k", "rf64" };
	for (auto & p : std::experimental::filesystem::directory_iterator("assets/audio"))
	{
		std::string file = p.path().string();
		auto dot = file.find_last_of('.');
		if (dot == std::string::npos)
			continue;
		std::string ext = file.substr(dot + 1);
		std::cout << "ext: " << ext << "\n";
		if (supportedFormats.count(ext))
		{
			m_musics.push_back(file);
		}
	}
	for (auto & s : m_musics)
	{
		std::cout << s << "\n";
	}
	loadNextMusic();
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
				unpacker.unpack(info.numPlayers);

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
	if (!m_musics.empty())
	{
		sf::Music::Status status = m_music.getStatus();
		if (status == sf::Music::Stopped && m_prevStatus == sf::Music::Playing)
		{
			loadNextMusic();
		}
		m_prevStatus = status;

	}
}

void LobbyScreen::render(Client & client)
{
	float dx = m_clock.restart().asSeconds() * 100.f;
	m_backgroundTextureRect.left -= dx;

	sf::IntRect r = static_cast<sf::IntRect>(m_backgroundTextureRect);
	m_background.setTextureRect(r);


	client.getWindow().draw(m_background);
}


void LobbyScreen::onExit(Client & client)
{

}

void LobbyScreen::onObscure(Client & client)
{
	m_tabs->hide();
	for (auto & p : m_panels)
		p->hide();
	m_musicPanel->hide();
	auto fadeOut = [this]()
	{
		sf::Clock clock;
		while (m_music.getVolume() > 5)
		{
			float dv = clock.restart().asSeconds() * 50.f;
			m_music.setVolume(m_music.getVolume() - dv);
		}
		m_music.stop();
		m_music.setVolume(100.f);

	};
	std::thread th(fadeOut);
	th.detach();
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
	auto bottomPanel = m_panels[INTERNET]->get<tgui::ScrollablePanel>("bottomPanel");
	std::size_t i = 0;
	
	auto grid = bottomPanel->get<tgui::Grid>("grid");
	grid->removeAllWidgets();

	for (const auto & info : m_internetGames)
	{
		float scrollbarWidth = 0.f;
		if (m_internetGames.size() * bottomPanel->getSize().y / 10 > bottomPanel->getSize().y)
			scrollbarWidth = bottomPanel->getScrollbarWidth();
		auto line = tgui::Panel::create({ tgui::bindWidth(bottomPanel) - scrollbarWidth - 4, tgui::bindHeight(bottomPanel) / 10 });
		
		auto onEnter = [line]()
		{
			line->getRenderer()->setOpacity(1.f);

			for (auto & w : line->getWidgets())
			{
				w->setInheritedOpacity(1.f);
				std::static_pointer_cast<tgui::Label>(w)->getRenderer()->setTextColor(sf::Color::Black);
			}
		};

		auto onLeave = [line]()
		{
			line->getRenderer()->setOpacity(.6f);
			for (auto & w : line->getWidgets())
			{
				w->setInheritedOpacity(1.f);
				std::static_pointer_cast<tgui::Label>(w)->getRenderer()->setTextColor(sf::Color::Black);
			}

		};

		auto onClick = [&info, &client]()
		{
			client.getNetwork().connect(info.addr);
		};
		line->onMouseEnter.connect(onEnter);
		line->onMouseLeave.connect(onLeave);
		line->onClick.connect(onClick);
		line->getRenderer()->setOpacity(.6f);

		if (i % 2)
			line->getRenderer()->setBackgroundColor(sf::Color(173, 216, 230));
		else
			line->getRenderer()->setBackgroundColor(sf::Color(176, 224, 230));

		auto name = tgui::Label::create(info.name);
		name->setSize("50%", "100%");
		name->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Left);
		name->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
		line->add(name, "name");
		name->setInheritedOpacity(1.f);
		name->getRenderer()->setTextColor(sf::Color::Black);

		auto mode = tgui::Label::create(info.modeName);
		mode->setPosition({ tgui::bindRight(name), tgui::bindTop(name) });
		mode->setSize({ "12.5%", "100%" });
		mode->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Left);
		mode->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
		line->add(mode, "mode");
		mode->setInheritedOpacity(1.f);
		mode->getRenderer()->setTextColor(sf::Color::Black);

		auto status = tgui::Label::create(info.status == GameInfo::Status::IN_GAME ? "In game" : "Waiting");
		status->setPosition({ tgui::bindRight(mode), tgui::bindTop(mode) });
		status->setSize({ "12.5%", "100%" });
		status->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Left);
		status->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
		line->add(status, "status");
		status->setInheritedOpacity(1.f);
		status->getRenderer()->setTextColor(sf::Color::Black);

		auto players = tgui::Label::create(std::to_string(info.numPlayers));
		players->setPosition({ tgui::bindRight(status), tgui::bindTop(status) });
		players->setSize({ "12.5%", "100%" });
		players->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Left);
		players->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
		line->add(players, "players");	
		players->setInheritedOpacity(1.f);
		players->getRenderer()->setTextColor(sf::Color::Black);

		auto ping = tgui::Label::create("10");
		ping->setSize({ "12.5%", "100%" });
		ping->setPosition({ tgui::bindRight(players), tgui::bindTop(mode) });
		ping->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Left);
		ping->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
		line->add(ping, "ping");
		ping->setInheritedOpacity(1.f);
		ping->getRenderer()->setTextColor(sf::Color::Black);

		grid->addWidget(line, i, 0);
		++i;
	}

	
}

void LobbyScreen::loadPrevMusic()
{
	if (m_musics.empty())
		return;

	bool wasPaused = m_music.getStatus() == sf::Music::Paused;
	m_currentMusicIndex = (m_currentMusicIndex - 1) % m_musics.size();
	m_music.openFromFile(m_musics[m_currentMusicIndex]);
	if(!wasPaused)
		m_music.play();
	std::cout << "prev\n";
}

void LobbyScreen::loadNextMusic()
{
	if (m_musics.empty())
		return;
	bool wasPaused = m_music.getStatus() == sf::Music::Paused;
	m_currentMusicIndex = (m_currentMusicIndex + 1) % m_musics.size();
	m_music.openFromFile(m_musics[m_currentMusicIndex]);
	if(!wasPaused)
		m_music.play();
	std::cout << "next\n";
}

