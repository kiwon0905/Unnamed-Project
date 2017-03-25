#pragma once
#include "AssetManager.h"
#include <SFML/Graphics.hpp>

class Client;

class Renderer
{
public:
	bool initialize(Client & client);
	void finalize(Client & client);

	void setTarget(sf::RenderTarget * target);
	void renderText(const std::string & s, float x, float y, unsigned size = 30, sf::Color color = sf::Color::White);
	void renderHuman(float x, float y);
private:
	sf::RenderTarget * m_target;
	AssetManager * m_assetManager;
};