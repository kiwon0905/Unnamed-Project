#pragma once


#include "AssetManager.h"
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
	struct Context
	{
		sf::RenderWindow window;
		Parser parser;
		AssetManager assetManager;
	};

	void run();

	Context & getContext() {return m_context;}
	Network & getNetwork() { return m_network; }
	ScreenStack & getScreenStack() { return m_screenStack; }
	Renderer & getRenderer() { return m_renderer; }
	Gui & getGui() { return m_gui; }
	Input & getInput() { return m_input; }

	float t;
private:
	bool initialize();
	void finalize();
private:
	Context m_context;
	Network m_network;
	Gui m_gui;
	ScreenStack m_screenStack;
	Renderer m_renderer;
	Input m_input;
};