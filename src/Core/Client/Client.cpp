#include "Client.h"
#include "LobbyScreen.h"
#include "Core/Logger.h"
#include "Core/ENetUtility.h"

#include <thread>
const sf::Time Client::TIME_STEP = sf::seconds(1 / 60.f);

bool Client::initialize()
{
	m_context = std::make_unique<Context>();

	if (!m_context->parser.loadFromFile("client-config.txt"))
	{
		Logger::getInstance().error("Failed to load client-config.txt");
		return false;
	}
	
	m_network = std::make_unique<Network>();
	if (!m_network->initialize(*this))
		return false;
	
	m_renderer = std::make_unique<Renderer>();
	if (!m_renderer->initialize(*this))
		return false;

	m_gui = std::make_unique<Gui>();
	if (!m_gui->initialize(*this))
		return false;

	m_screenStack = std::make_unique<ScreenStack>();
	if (!m_screenStack->initialize(*this))
		return false;

	m_input = std::make_unique<Input>();
	if (!m_input->initialize(*this))
		return false;
	std::cout << "init success\n";
	return true;
}

void Client::run()
{
	if (initialize())
	{
		sf::Clock clock;
		sf::Time elapsed = sf::Time::Zero;
		m_screenStack->push(new LobbyScreen);
		m_screenStack->applyChanges(*this);

		sf::Clock fpsClock;
		int frames = 0;
		while (!m_screenStack->isEmpty())
		{
			elapsed += clock.restart();
			while (elapsed >= TIME_STEP)
			{
				elapsed -= TIME_STEP;

				//handle input event
				sf::Event event;
				while (m_context->window.pollEvent(event))
				{
					if (event.type == sf::Event::Closed)
						m_screenStack->clear();
					m_gui->handleEvent(event);
					m_screenStack->handleEvent(event, *this);
				}

				NetEvent * netEvent;
				while (netEvent = m_network->peekEvent())
				{
					m_screenStack->handleNetEvent(*netEvent, *this);
					m_network->popEvent();
				}

				Packet packet;
				ENetAddress addr;
				while (enutil::receive(getNetwork()->getSocket(), addr, packet) > 0)
					m_screenStack->handlePacket(packet, addr, *this);


				m_input->update();
				m_gui->tick(TIME_STEP.asSeconds(), *this);
				m_screenStack->tick(TIME_STEP.asSeconds(), *this);

			}
			m_context->window.clear(sf::Color::Cyan);
				m_screenStack->render(*this);
				m_gui->render(*this);
			m_context->window.display();
		
			m_screenStack->applyChanges(*this);

		}
	}
	finalize();
}

void Client::finalize()
{
	m_input->finalize(*this);
	m_screenStack->finalize(*this);
	m_gui->finalize(*this);
	m_renderer->finalize(*this);
	m_network->finalize(*this);
}

