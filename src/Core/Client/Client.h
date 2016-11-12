#pragma once


#include "ResourceManager.h"
#include "Network.h"
#include "Gui.h"
#include "ScreenStack.h"
#include "Renderer.h"
#include "Input.h"

#include "Core/Parser.h"
#include <SFML/Graphics.hpp>
#include <memory>


class Client
{
public:
	const static sf::Time TIME_STEP;
	struct Context
	{
		sf::RenderWindow window;
		Parser parser;
		ResourceManager resourceManager;
	};

	void run();

	Context * getContext() {return m_context.get();}
	Network * getNetwork() { return m_network.get(); }
	ScreenStack * getScreenStack() { return m_screenStack.get(); }
	Renderer * getRenderer() { return m_renderer.get(); }
	Gui * getGui() { return m_gui.get(); }
	Input * getInput() { return m_input.get(); }

private:
	bool initialize();
	void finalize();
private:
	std::unique_ptr<Context> m_context;
	std::unique_ptr<Network> m_network;
	std::unique_ptr<Gui> m_gui;
	std::unique_ptr<ScreenStack> m_screenStack;
	std::unique_ptr<Renderer> m_renderer;
	std::unique_ptr<Input> m_input;
};