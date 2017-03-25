#include "Client.h"
#include "LobbyScreen.h"
#include "Game/GameCore.h"
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
	
	if (!m_renderer.initialize(*this))
		return false;

	if (!m_gui.initialize(*this))
		return false;

	if (!m_screenStack.initialize(*this))
		return false;

	if (!m_input.initialize(*this))
		return false;
	Logger::getInstance().info("Initialization successful");


	//window.setFramerateLimit(300);
	return true;
}

void Client::run()
{
	if (initialize())
	{
		sf::Clock clock;
		sf::Time elapsed = sf::Time::Zero;
		m_screenStack.push(new LobbyScreen);
		m_screenStack.applyChanges(*this);

		sf::Clock fpsClock;
		int frames = 0;
		int fps = 0;
		const sf::Time tickRate = sf::seconds(1.f / TICK_RATE);
		while (!m_screenStack.isEmpty())
		{
			sf::Time dt = clock.restart();
			elapsed += dt;
			while (elapsed >= sf::seconds(1.f /TICK_RATE))
			{
				elapsed -= tickRate;
				//handle input event
				m_gui.update(tickRate.asSeconds(), *this);
				m_screenStack.update(tickRate.asSeconds(), *this);
			}
			
			m_frameProgress = static_cast<float>(elapsed.asMicroseconds()) / tickRate.asMicroseconds();
			sf::Event event;
			while (m_context.window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					m_screenStack.clear();
				m_gui.handleEvent(event);
				m_screenStack.handleEvent(event, *this);
			}
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



			frames++;
			if (fpsClock.getElapsedTime() > sf::seconds(1.f))
			{
				fps = frames * 1;
				frames = 0;
				fpsClock.restart();
			}
			m_context.window.clear();
			m_screenStack.render(*this);

			m_renderer.setTarget(&m_context.window);
			m_gui.render(*this);
			m_renderer.renderText("FPS: " + std::to_string(fps), 0.f, 0.f);
			m_context.window.display();
			m_screenStack.applyChanges(*this);

			std::this_thread::sleep_for(std::chrono::microseconds(100));

		}
	}
	finalize();
}

void Client::finalize()
{
	m_input.finalize(*this);
	m_screenStack.finalize(*this);
	m_gui.finalize(*this);
	m_renderer.finalize(*this);
	m_network.finalize(*this);
}

