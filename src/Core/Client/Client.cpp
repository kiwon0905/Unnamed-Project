#include "Client.h"
#include "LobbyScreen.h"
#include "Game/GameConfig.h"
#include "Core/Logger.h"
#include "Core/ENetUtility.h"


#include <thread>

bool Client::initialize()
{

	sf::RenderWindow & window = getContext().window;
	window.create(sf::VideoMode(1600, 900), "");
	window.resetGLStates();

	if (!m_context.parser.loadFromFile("client-config.txt"))
	{
		Logger::getInstance().error("Failed to load client-config.txt");
		return false;
	}
	
	if (!m_network.initialize(*this))
		return false;
	
	if (!m_gui.initialize(*this))
		return false;

	if (!m_screenStack.initialize(*this))
		return false;

	if (!m_input.initialize(*this))
		return false;
	Logger::getInstance().info("Initialization successful");

	m_screenStack.push(new LobbyScreen);
	m_screenStack.applyChanges(*this);
	//window.setFramerateLimit(300);
	return true;
}

void Client::run()
{
	if (initialize())
	{
		sf::Clock clock;


		while (!m_screenStack.isEmpty())
		{
			sf::Time dt = clock.restart();
			m_frameTime = dt;
			if (dt < m_minFrameTime)
				m_minFrameTime = dt;
			if (dt > m_maxFrameTime)
				m_maxFrameTime = dt;

			sf::Event event;
			while (m_context.window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					m_screenStack.clear();
				m_gui.handleEvent(event);
				m_screenStack.handleEvent(event, *this);
			}

			m_gui.update(dt.asSeconds(), *this);


			m_network.update();
			ENetEvent * netEvent;
			while (netEvent = m_network.peekEvent())
			{
				m_screenStack.handleNetEvent(*netEvent, *this);
				m_network.popEvent();
			}

			/*	Unpacker unpacker;
			ENetAddress addr;
			while (m_network.receive(unpacker, addr))
			m_screenStack.handlePacket(unpacker, addr, *this);*/


			m_screenStack.update(*this);
			
			m_context.window.clear();
			m_screenStack.render(*this);
			m_gui.render(*this);
			m_context.window.display();

			m_screenStack.applyChanges(*this);
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
	finalize();
}

void Client::finalize()
{
	m_input.finalize(*this);
	m_screenStack.finalize(*this);
	m_gui.finalize(*this);
	m_network.finalize(*this);
}

