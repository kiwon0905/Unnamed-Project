#include "LobbyScreen.h"
#include "Client.h"
#include "RoomScreen.h"
#include "Core/Logger.h"
#include "Core/Packer.h"
#include "Core/Protocol.h"
#include "Core/ENetUtility.h"

#include <SFGUI/Widgets.hpp>

constexpr unsigned GAMES_PER_PAGE = 4;

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
	Msg msg;
	unpacker.unpack(msg);

	if (msg == Msg::MSV_INTERNET_SERVER_LIST)
	{
		m_internetGameServers.clear();
		std::uint32_t size;
		unpacker.unpack(size);
		Logger::getInstance().debug(std::to_string(size) + " servers");
		for (std::size_t i = 0; i < size; ++i)
		{
			ServerInfo info;
			unpacker.unpack(info.address.host);
			unpacker.unpack(info.address.port);
			unpacker.unpack(info.id);
			unpacker.unpack(info.name);
			m_internetGameServers.push_back(info);
			Logger::getInstance().info(enutil::toString(info.address));
		}
		turnPage(0, client);
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
	hideUi(client);
	std::cout << "lobby exit!";
}

void LobbyScreen::onObscure(Client & client)
{
	hideUi(client);
}

void LobbyScreen::onReveal(Client & client)
{
	m_window->Show(true);
}


//Window
//	Box2
//		Notebook
//			Box(
//			InternetTable
//			Box(Prev, Next, Refresh)
//			)
//			Private
//

void LobbyScreen::loadUi(Client & client)
{

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
	auto buttonBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
	buttonBox->Pack(prevButton);
	buttonBox->Pack(nextButton);
	buttonBox->Pack(refreshButton);


	m_internetTable = sfg::Table::Create();
	for (std::size_t i = 0; i < GAMES_PER_PAGE / 2; ++i)
		for (std::size_t j = 0; j < 2; ++j)
			m_internetTable->Attach(sfg::Button::Create(), { j, i, 1, 1 });

	sfg::Box::Ptr box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
	box->Pack(m_internetTable);
	box->Pack(buttonBox);

	m_notebook = sfg::Notebook::Create();
	m_notebook->AppendPage(box, sfg::Label::Create("Internet"));
	

	sfg::Box::Ptr box2 = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
	sfg::Entry::Ptr entry = sfg::Entry::Create();
	box2->Pack(entry);
	sfg::Button::Ptr joinButton = sfg::Button::Create("Join");
	auto joinPrivate = [&client, entry]()
	{
		ENetAddress addr = enutil::toENetAddress(entry->GetText().toAnsiString());

		if (!client.getNetwork().connect(addr))
			Logger::getInstance().info("Already connecting");
	};
	joinButton->GetSignal(sfg::Button::OnLeftClick).Connect(joinPrivate);
	box2->Pack(joinButton);
	m_notebook->AppendPage(box2, sfg::Label::Create("Private"));


	m_window = sfg::Window::Create();
	m_window->Add(m_notebook);


	auto & desktop = client.getGui().getDesktop();
	desktop.Add(m_window);
}

void LobbyScreen::hideUi(Client & client)
{
	m_window->Show(false);
}

void LobbyScreen::refresh(Client & client)
{
	m_internetGameServers.clear();
	for (auto & child : m_internetTable->GetChildren())
	{
		std::static_pointer_cast<sfg::Button>(child)->SetLabel("");
	}


	std::string addr;
	client.getContext().parser.get("masterAddr", addr);
	ENetAddress address = enutil::toENetAddress(addr);
	
	Packer packer;
	packer.pack(Msg::CL_REQUEST_INTERNET_SERVER_LIST);
	client.getNetwork().send(packer, address);
}

void LobbyScreen::turnPage(int page, Client & client)
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
				client.getNetwork().connect(serverInfo.address);
			};
			std::static_pointer_cast<sfg::Button>(child)->GetSignal(sfg::Button::OnLeftClick).Connect(onClick);

		}
		//	std::cout << "i: " << i << "j: " << j << "\n";
		index++;
	}
}
