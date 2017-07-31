#pragma once


#include "AssetManager.h"
#include "SoundPlayer.h"
#include "Network.h"
#include "ScreenStack.h"
#include "Input.h"
#include "Core/Parser.h"
#include <SFML/Graphics.hpp>
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Desktop.hpp>

class Client
{
public:
	void run();

	Parser & getParser() { return m_parser; }
	sf::RenderWindow & getWindow() { return m_window; }
	AssetManager & getAssetManager() { return m_assetManager; }
	SoundPlayer & getSoundPlayer() { return m_soundPlayer; }
	Network & getNetwork() { return m_network; }
	ScreenStack & getScreenStack() { return m_screenStack; }
	sfg::Desktop & getDesktop() { return m_desktop; }
	Input & getInput() { return m_input; }

	bool debugRenderEnabled() { return m_debugRender; }
private:
	bool initialize();
	void finalize();
private:
	Parser m_parser;
	sf::RenderWindow m_window;
	AssetManager m_assetManager;
	SoundPlayer m_soundPlayer;
	Network m_network;
	sfg::Desktop m_desktop;
	sfg::SFGUI m_gui;
	ScreenStack m_screenStack;
	Input m_input;
	bool m_debugRender = false;
};