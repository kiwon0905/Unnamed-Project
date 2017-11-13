#include "LobbyScreen.h"
#include "Client.h"
#include "RoomScreen.h"

#include "Core/Logger.h"
#include "Core/Packer.h"
#include "Core/Protocol.h"
#include "Core/ENetUtility.h"

#include <filesystem>
#include <random>


LobbyScreen::LobbyScreen()
{
	m_panels.resize(COUNT);
}

void LobbyScreen::onEnter(Client & client)
{
	auto & window = client.getWindow();
	auto & gui = client.getGui();

	sf::Texture * backgroundTexture = client.getAssetManager().get<sf::Texture>("assets/colored_talltrees.png");
	backgroundTexture->setRepeated(true);
	backgroundTexture->setSmooth(true);

	m_backgroundTextureRect.width = static_cast<float>(client.getWindow().getSize().x) / client.getWindow().getSize().y * backgroundTexture->getSize().y;
	m_backgroundTextureRect.height = static_cast<float>(backgroundTexture->getSize().y);
	m_background.setTexture(*backgroundTexture);

	m_tabs = tgui::Tabs::create();
	gui.add(m_tabs);
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
		p = tgui::Panel::create({ "80%", "60%" });
		p->setPosition({ "(&.width - width) / 2", "(&.height - height) / 3" });
		p->hide();
		gui.add(p);
	}
	m_panels[INTERNET]->show();

	m_tabs->setPosition({ tgui::bindLeft(m_panels[INTERNET]), tgui::bindTop(m_panels[INTERNET]) - m_tabs->getSize().y - 2});

	//INTERNET
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


	//SETTINGS
	m_panels[SETTINGS]->getRenderer()->setBackgroundColor(sf::Color::Transparent);
	m_panels[SETTINGS]->getRenderer()->setBorders(2.f);
	m_panels[SETTINGS]->getRenderer()->setBorderColor(sf::Color::Black);
	
	auto nameLabel = tgui::Label::create("Name:");
	//nameLabel->getRenderer()->setBorders(1.f);
	//nameLabel->getRenderer()->setBorderColor(sf::Color::Black);
	m_panels[SETTINGS]->add(nameLabel);


	auto nameEditBox = tgui::EditBox::create();
	nameEditBox->getRenderer()->setBackgroundColor(sf::Color::Transparent);
	nameEditBox->getRenderer()->setBorders(0.f);
	nameEditBox->setPosition({ tgui::bindRight(nameLabel) + 2, tgui::bindTop(nameLabel) });
	nameEditBox->setSize({ "30%", tgui::bindHeight(nameLabel) });
	nameEditBox->setTextSize(nameLabel->getTextSize());
	nameEditBox->setDefaultText("Unnamed Person");
	m_panels[SETTINGS]->add(nameEditBox, "nameEditBox");
	//music
	m_musicPanel = tgui::Panel::create({ "parent.width * .2", "parent.width * .05" });
	m_musicPanel->setPosition({ "40%", tgui::bindBottom(m_panels[INTERNET]) + tgui::bindBottom(m_panels[INTERNET]) * .1});
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


	//load music
	std::unordered_set<std::string> supportedFormats = { "ogg", "wav", "flac", "aiff", "au", "raw",
		"paf", "svx", "nist", "voc", "ircam", "w64", "mat4", "mat5", "pvf", "htk", "sds", "avr", "sd2", "caf", "wve", "mpc2k", "rf64" };
	for (auto & p : std::experimental::filesystem::directory_iterator("assets/audio"))
	{
		std::string file = p.path().string();
		auto dot = file.find_last_of('.');
		if (dot == std::string::npos)
			continue;
		std::string ext = file.substr(dot + 1);
		if (supportedFormats.count(ext))
		{
			m_musics.push_back(file);
		}
	}

	//shuffle music
	auto seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle(m_musics.begin(), m_musics.end(), std::default_random_engine(static_cast<unsigned>(seed)));

	loadNextMusic();
	requestInternetGamesInfo(client);
}

void LobbyScreen::handleEvent(const sf::Event & event, Client & client)
{
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R && event.key.control)
	{
		requestInternetGamesInfo(client);

	}
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P && event.key.control)
	{
		checkPing(client);
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
		std::string name = m_panels[SETTINGS]->get<tgui::EditBox>("nameEditBox")->getText();
		if (name.empty())
			name = m_panels[SETTINGS]->get<tgui::EditBox>("nameEditBox")->getDefaultText();
		{
			Logger::getInstance().info("LobbyScreen", "Connected to game server");
			Packer packer;

			packer.pack(Msg::CL_REQUEST_JOIN_GAME);
			packer.pack(name);

			client.getNetwork().send(packer, true);
		}
	}
	else if (netEv.type == ENET_EVENT_TYPE_DISCONNECT)
	{
		Logger::getInstance().info("LobbyScreen", "Disconnected from game server");
	}

}

void LobbyScreen::handleUdpPacket(Unpacker & unpacker, const ENetAddress & addr, Client & client)
{
	Msg msg;
	unpacker.unpack(msg);
	if (client.getMasterServerAddress() && client.getMasterServerAddress()->host == addr.host && client.getMasterServerAddress()->port == addr.port)
	{
		std::cout << "msg: " << (int)msg << "\n";

		if (msg == Msg::MSV_INTERNET_SERVER_INFO)
		{
			m_internetGames.clear();
			
			std::size_t count;
			unpacker.unpack(count);
			std::cout << "count: " << count << "\n";
			for (std::size_t i = 0; i < count; ++i)
			{
				GameInfo info;
				unpacker.unpack(info.addr.host);
				unpacker.unpack(info.addr.port);	//game server port
				unpacker.unpack(info.pingCheckPort); // ping check port
				unpacker.unpack(info.id);
				unpacker.unpack(info.name);
				unpacker.unpack(info.modeName);
				unpacker.unpack(info.status);
				unpacker.unpack(info.numPlayers);
				m_internetGames.push_back(info);
			}

			updateInternetGamesUi(client);
			//checkPing(client);
		}
	}

	else if (msg == Msg::SV_PING_REPLY)
	{

		for (auto & g : m_internetGames)
		{
			if (g.addr.host == addr.host && g.pingCheckPort == addr.port)
			{
				auto bottomPanel = m_panels[INTERNET]->get<tgui::ScrollablePanel>("bottomPanel");
				auto grid = bottomPanel->get<tgui::Grid>("grid");
				std::string s;
				enutil::toString(g.addr, s);
				auto line = grid->get<tgui::Panel>(s);

				line->get<tgui::Label>("ping")->setText(std::to_string(g.lastPingReq.getElapsedTime().asMilliseconds()));
				break;
			}
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

	sf::IntRect r = sf::IntRect(static_cast<int>(m_backgroundTextureRect.left + .5f), 
								static_cast<int>(m_backgroundTextureRect.top + .5f),
								static_cast<int>(m_backgroundTextureRect.width + .5f),
								static_cast<int>(m_backgroundTextureRect.height + .5f));
	m_background.setTextureRect(r);

	sf::View view;
	view.setSize(m_backgroundTextureRect.width + .5f, m_backgroundTextureRect.height + .5f);
	view.setCenter(view.getSize() / 2.f);
	sf::View temp = client.getWindow().getView();
	client.getWindow().setView(view);
	
	client.getWindow().draw(m_background);
	client.getWindow().setView(temp);
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

		auto ping = tgui::Label::create("-");
		ping->setSize({ "12.5%", "100%" });
		ping->setPosition({ tgui::bindRight(players), tgui::bindTop(mode) });
		ping->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Left);
		ping->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
		line->add(ping, "ping");
		ping->setInheritedOpacity(1.f);
		ping->getRenderer()->setTextColor(sf::Color::Black);

		grid->addWidget(line, i, 0);

		std::string s;
		enutil::toString(info.addr, s);
		grid->setWidgetName(line, s);
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
}

void LobbyScreen::checkPing(Client & client)
{
	Packer packer;
	packer.pack(Msg::CL_PING);
	for (auto & g : m_internetGames)
	{
		ENetAddress addr = g.addr;
		addr.port = g.pingCheckPort;
		client.getNetwork().send(packer, addr);
		g.lastPingReq.restart();

	}
}

