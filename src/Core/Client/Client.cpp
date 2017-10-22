#include "Client.h"
#include "LobbyScreen.h"
#include "Game/GameConfig.h"
#include "Graph.h"
#include "Core/Logger.h"
#include "Core/ENetUtility.h"


#include <thread>

bool Client::initialize()
{
	m_window.create(sf::VideoMode(1600, 900), "", sf::Style::Default, sf::ContextSettings(0, 0, 8));
	m_window.resetGLStates();

	if (!m_parser.loadFromFile("client-config.txt"))
	{
		Logger::getInstance().error("Client", "Failed to load client-config.txt");
		return false;
	}

	if (!m_network.initialize(*this))
	{
		Logger::getInstance().error("Client", "Failed to initialize network");
		return false;
	}

	
	std::string masterAddr;
	ENetAddress enetMasterAddr;
	if (!m_parser.get("masterAddr", masterAddr) || !enutil::toENetAddress(masterAddr, enetMasterAddr))
	{
		Logger::getInstance().warn("Client", "Failed to read master server address. Internet servers are unavailable");
	}
	else
	{
		Logger::getInstance().info("Client", "Master server address: " + masterAddr);
		m_masterServerAddress.reset(new ENetAddress);
		*m_masterServerAddress = enetMasterAddr;
	}

	if (!m_screenStack.initialize(*this))
		return false;

	if (!m_input.initialize(*this))
		return false;

	Logger::getInstance().info("Client", "Initialization successful");

	m_screenStack.push(new LobbyScreen);
	m_screenStack.applyChanges(*this);
	//m_window.setVerticalSyncEnabled(true);
	//window.setFramerateLimit(300);
	//window.setMouseCursorGrabbed(true);

	m_gui.setTarget(m_window);
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

		const sf::Font * font = m_assetManager.get<sf::Font>("assets/font/arial.ttf");
		Graph frameTimeGraph(0.f, 20.f, *font, "Frame time(ms)");
		frameTimeGraph.setSize({ 200.f, 100.f });
		
		while (!m_screenStack.isEmpty())
		{
			old = current;
			current = clock.getElapsedTime();
			sf::Time dt = current - old;
			
			frameTimeGraph.addSample(dt.asMicroseconds() / 1000.f);
			
			fpsAccumulator += dt;
			if (fpsAccumulator > sf::seconds(1.f))
			{
				m_window.setTitle("FPS: " + std::to_string(numFrames));
				numFrames = 0;
				fpsAccumulator = sf::Time::Zero;
			}
			numFrames++;	

			sf::Event event;
			while (m_window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					m_screenStack.clear();
				m_screenStack.handleEvent(event, *this);
				m_gui.handleEvent(event);
				m_input.handleEvent(event);		
			}

			if (m_input.isActive({ sf::Keyboard::LControl, sf::Keyboard::LShift, sf::Keyboard::D }))
			{
				m_debugRender ^= 1;
			}

			m_network.update();
			ENetEvent * netEvent;
			while (netEvent = m_network.peekEvent())
			{
				m_screenStack.handleNetEvent(*netEvent, *this);
				m_network.popEvent();
			}

			Unpacker unpacker;
			ENetAddress addr;
			while (m_network.receive(unpacker, addr))
				m_screenStack.handleUdpPacket(unpacker, addr, *this);

			m_screenStack.update(*this);

			
			m_window.clear(sf::Color::Cyan);
			m_screenStack.render(*this);
			m_gui.draw();

			if(m_debugRender)
				m_window.draw(frameTimeGraph);

			m_window.display();

			std::this_thread::sleep_for(std::chrono::microseconds(10));
			//sf::sleep(sf::microseconds(1));
			m_screenStack.applyChanges(*this);
		}
	}
	finalize();
}

void Client::finalize()
{
	m_input.finalize(*this);
	m_screenStack.finalize(*this);
	m_network.finalize(*this);
}

