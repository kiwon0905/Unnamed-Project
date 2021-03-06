#include "ScreenStack.h"
#include "LobbyScreen.h"
#include "Client.h"
#include "Core/ENetUtility.h"

bool ScreenStack::initialize(Client & client)
{
	return true;
}

void ScreenStack::finalize(Client & client)
{

}

void ScreenStack::update(Client & client)
{
	m_screens.top()->update(client);
}

void ScreenStack::render(Client & client)
{
	m_screens.top()->render(client);
}

void ScreenStack::pop()
{
	m_changes.push_back(Change{ -1, nullptr });
	std::cout << "POP\n";
}

void ScreenStack::push(Screen * screen)
{
	m_changes.push_back(Change{ 1, screen });
	std::cout << "PUSH!\n";
}

void ScreenStack::clear()
{
	m_changes.push_back(Change{ 0, nullptr });
	std::cout << "CLEAR\n";
}

bool ScreenStack::isEmpty()
{
	return m_screens.empty();
}

void ScreenStack::handleEvent(const sf::Event & event, Client & client)
{
	m_screens.top()->handleEvent(event, client);
}

void ScreenStack::handleNetEvent(ENetEvent & netEv, Client & client)
{
	m_screens.top()->handleNetEvent(netEv, client);
}

void ScreenStack::handleUdpPacket(Unpacker & unpacker, const ENetAddress & addr, Client & client)
{
	m_screens.top()->handleUdpPacket(unpacker, addr, client);
}

void ScreenStack::applyChanges(Client & client)
{
	for (const auto & change : m_changes)
	{
		if (change.action == 1)
		{
			if (m_screens.size() > 0)
				m_screens.top()->onObscure(client);
			m_screens.push(std::unique_ptr<Screen>(change.screen));
			m_screens.top()->onEnter(client);
		}
		else if (change.action == -1)
		{
			if (m_screens.size() > 0)
			{
				m_screens.top()->onExit(client);
				m_screens.pop();
				if (m_screens.size() > 0)
					m_screens.top()->onReveal(client);
			}
		}
		else if (change.action == 0)
		{
			while (m_screens.size() > 1)
			{
				m_screens.top()->onExit(client);
				m_screens.pop();
				m_screens.top()->onReveal(client);
			}
			m_screens.top()->onExit(client);
			m_screens.pop();
		}
	}
	m_changes.clear();
}