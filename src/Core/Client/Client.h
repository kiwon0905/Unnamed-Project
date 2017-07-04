#pragma once


#include "AssetManager.h"
#include "SoundPlayer.h"
#include "Network.h"
#include "Gui.h"
#include "ScreenStack.h"
#include "Input.h"

#include "Core/Parser.h"
#include <SFML/Graphics.hpp>
#include <memory>


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
	Gui & getGui() { return m_gui; }
	Input & getInput() { return m_input; }

private:
	bool initialize();
	void finalize();
private:
	Parser m_parser;
	sf::RenderWindow m_window;
	AssetManager m_assetManager;
	SoundPlayer m_soundPlayer;
	Network m_network;
	Gui m_gui;
	ScreenStack m_screenStack;
	Input m_input;
};