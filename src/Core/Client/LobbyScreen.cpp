#include "LobbyScreen.h"
#include "Client.h"
#include "PlayingScreen.h"
#include "Core/Logger.h"
#include "Core/Packet.h"
#include "Core/Protocol.h"
#include "Core/ENetUtility.h"

#include <SFGUI/Widgets.hpp>

constexpr unsigned GAMES_PER_PAGE = 4;

void LobbyScreen::onEnter(Client & client)
{
	Logger::getInstance().info("client entered");
	loadUi(client);

	/*auto requestJob = [&client]()
	{
		Packet packet;
		packet << Msg::CL_REQUEST_INTERNET_SERVER_LIST;
		ENetAddress addr = enutil::toENetAddress("localhost", 12344);
		int i = enutil::send(client.getNetworkManager().getSocket(), addr, packet);
	};
	m_jobScheduler.addJob(requestJob, 2.f);*/
}

void LobbyScreen::handleEvent(const sf::Event & ev, Client & client)
{
}

void LobbyScreen::handleNetEvent(NetEvent & netEv, Client & client)
{
	if (netEv.type == NetEvent::Received)
	{
		Msg msg;
		*netEv.packet >> msg;
		if (msg == Msg::SV_ACCEPT_JOIN)
		{
			std::cout << "joined game\n";
			client.getScreenStack()->push(new PlayingScreen);
		}
		else if (msg == Msg::SV_REJECT_JOIN)
		{
			std::cout << "rejected from joining game\n";
			client.getNetwork()->disconnect();
		}
	}
	else if (netEv.type == NetEvent::Connected)
	{
		std::cout << "conencted to game\n";
		Packet packet;
		packet << Msg::CL_REQUEST_JOIN_GAME;
		client.getNetwork()->send(packet, true);
	}
	else if (netEv.type == NetEvent::Disconnected)
	{
		std::cout << "disconnected\n";
	}
	else if (netEv.type == NetEvent::TimedOut)
	{
		std::cout << "timed out\n";
	}
}

void LobbyScreen::handlePacket(Packet & packet, const ENetAddress & addr, Client & client)
{
	Msg msg;
	packet >> msg;

	if (msg == Msg::MSV_INTERNET_SERVER_LIST)
	{
		m_internetGameServers.clear();
		sf::Uint32 size;
		packet >> size;
		std::cout << size << " servers\n";
		for (std::size_t i = 0; i < size; ++i)
		{
			ServerInfo info;
			packet >> info.address.host >> info.address.port >> info.id >> info.name;
			m_internetGameServers.push_back(info);
		}
	}
	else if (msg == Msg::SV_LAN_GAME_LIST)
	{
		ServerInfo info;
		packet >> info.address.port >> info.name;
		info.address.host = addr.host;
		std::cout << "Lan game discovered: " << enutil::toString(addr) << "\n";
	}
	turnPage(0, client);
}

void LobbyScreen::update(float dt, Client & client)
{
	m_jobScheduler.update(dt);
}

void LobbyScreen::render(Client & client)
{
//	Logger::getInstance().info() << client.getNetworkManager().getIncomingBandwidth() << " down " << client.getNetworkManager().getOutgoingBandwidth() << " up\n";
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


//Window
//	Box2
//		Notebox
//			InternetTable
//			LanTable
//		Box(Prev, Next, Refresh)

void LobbyScreen::loadUi(Client & client)
{
	sf::RenderWindow & window = client.getContext()->window;
	int width = sf::VideoMode::getDesktopMode().width * 4 / 7;
	int height = width * 9 / 16;
	window.create(sf::VideoMode(width, height), "");
	window.resetGLStates();
	window.setFramerateLimit(60);
	
	auto & desktop = client.getGui()->getDesktop();

	auto prevButton = sfg::Button::Create("Prev");
	auto onPrev = [this, &client]()
	{
		turnPage(-1, client);
	};
	prevButton->GetSignal(sfg::Button::OnLeftClick).Connect(onPrev);

	auto nextButton = sfg::Button::Create("Next");
	auto onNext = [this, &client]()
	{
		turnPage(1, client);
	};
	nextButton->GetSignal(sfg::Button::OnLeftClick).Connect(onNext);


	auto refreshButton = sfg::Button::Create("Refresh");
	refreshButton->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind(&LobbyScreen::refresh, this, std::ref(client)));
	auto box = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
	m_internetTable = sfg::Table::Create();
	for (std::size_t i = 0; i < GAMES_PER_PAGE / 2; ++i)
		for (std::size_t j = 0; j < 2; ++j)
			m_internetTable->Attach(sfg::Button::Create(), { j, i, 1, 1 });

	m_lanTable = sfg::Table::Create();
	for (std::size_t i = 0; i < GAMES_PER_PAGE / 2; ++i)
		for (std::size_t j = 0; j < 2; ++j)
			m_lanTable->Attach(sfg::Button::Create(), { j, i, 1, 1 });

	m_notebook = sfg::Notebook::Create();
	auto box2 = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
	m_window = sfg::Window::Create();
	//m_window->SetRequisition(sf::Vector2f(static_cast<float>(width), static_cast<float>(height)));
	m_window->Add(box2);
	box2->Pack(m_notebook);

	m_notebook->AppendPage(m_internetTable, sfg::Label::Create("Internet"));
	m_notebook->AppendPage(m_lanTable, sfg::Label::Create("LAN"));

	box2->Pack(box, false, true);
	box->Pack(prevButton, false, false);
	box->Pack(nextButton, false, false);
	box->Pack(refreshButton, false, false);
	desktop.Add(m_window);
}

void LobbyScreen::hideUi(Client & client)
{
	m_window->Show(false);
}

void LobbyScreen::refresh(Client & client)
{
	m_internetGameServers.clear();
	m_lanGameServers.clear();
	for (auto & child : m_internetTable->GetChildren())
	{
		std::static_pointer_cast<sfg::Button>(child)->SetLabel("");
	}
	for (auto & child : m_lanTable->GetChildren())
	{
		std::static_pointer_cast<sfg::Button>(child)->SetLabel("");
	}

	if (m_notebook->GetNthPage(m_notebook->GetCurrentPage()) == m_internetTable)
	{
		std::string addr;
		client.getContext()->parser.get("masterAddr", addr);
		ENetAddress address = enutil::toENetAddress(addr);

		Packet packet;
		packet << Msg::CL_REQUEST_INTERNET_SERVER_LIST;
		enutil::send(client.getNetwork()->getSocket(), address, packet);
		std::cout << "refreshed!\n";

	}
	else
	{


	}
}

void LobbyScreen::turnPage(int page, Client & client)
{
	if (m_notebook->GetNthPage(m_notebook->GetCurrentPage()) == m_internetTable)
	{
		for (auto & child : m_internetTable->GetChildren())
		{
			std::static_pointer_cast<sfg::Button>(child)->SetLabel("");
		}

		m_currentPage += page;
		m_currentPage = std::max(1, m_currentPage);
		int maxPage = m_internetGameServers.size() / GAMES_PER_PAGE;
		if (m_internetGameServers.size() % GAMES_PER_PAGE > 0)
			maxPage += 1;
		m_currentPage = std::min(m_currentPage, maxPage);

		std::size_t index = (m_currentPage - 1) * GAMES_PER_PAGE;
		
		for (auto & child : m_internetTable->GetChildren())
		{
			if (index < m_internetGameServers.size())
			{
				const ServerInfo & serverInfo = m_internetGameServers[index];
				std::static_pointer_cast<sfg::Button>(child)->SetLabel(serverInfo.name);

				auto onClick = [&client, &serverInfo]()
				{
					client.getNetwork()->connect(serverInfo.address);
				};
				std::static_pointer_cast<sfg::Button>(child)->GetSignal(sfg::Button::OnLeftClick).Connect(onClick);

			}
			//	std::cout << "i: " << i << "j: " << j << "\n";
			index++;
		}


	}
	else
	{


	}

}
