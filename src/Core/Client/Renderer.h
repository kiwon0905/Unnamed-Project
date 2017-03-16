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
	void renderText(const std::string & s, float x, float y);
	void renderHuman(float x, float y);
private:
	sf::RenderTarget * m_target;
	AssetManager * m_assetManager;
};