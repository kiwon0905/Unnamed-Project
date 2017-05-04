#include "Client.h"
#include "LobbyScreen.h"
#include "Game/GameConfig.h"
#include "Graph.h"
#include "Core/Logger.h"
#include "Core/ENetUtility.h"


#include <thread>

bool Client::initialize()
{

	sf::RenderWindow & window = getContext().window;
	window.create(sf::VideoMode(1600, 900), "", sf::Style::Default, sf::ContextSettings(0, 0, 8));
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
	window.setFramerateLimit(300);
	return true;
}

void Client::run()
{
	if (initialize())
	{
		sf::Clock clock;
		sf::Time old;
		sf::Time current;
		sf::Time fpsAccumulator;
		int numFrames = 0;

		const sf::Font * font = m_context.assetManager.get<sf::Font>("arial.ttf");
		Graph frameTimeGraph(0.f, 20.f, *font, "Frame time(ms)");
		frameTimeGraph.setSize({ 200.f, 100.f });
		
		while (!m_screenStack.isEmpty())
		{
			old = current;
			current = clock.getElapsedTime();

			sf::Time dt = current - old;
			frameTimeGraph.addSample(dt.asMicroseconds() / 1000.f);
			sf::Event event;
			
			fpsAccumulator += dt;
			if (fpsAccumulator > sf::seconds(1.f))
			{
				m_context.window.setTitle("FPS: " + std::to_string(numFrames));
				numFrames = 0;
				fpsAccumulator = sf::Time::Zero;
			}
			numFrames++;
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

			m_context.window.draw(frameTimeGraph);

			m_context.window.display();

			m_screenStack.applyChanges(*this);
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

