#pragma once
#include "AssetManager.h"
#include <SFML/Graphics.hpp>

class Client;

class Renderer
{
public:
	bool initialize(Client & client);
	void finalize(Client & client);

	void renderText(const std::string & s, float x, float y);
	void renderHuman(float x, float y);
private:
	sf::RenderWindow * m_window;
	AssetManager * m_assetManager;
};