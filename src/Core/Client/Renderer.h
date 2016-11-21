#pragma once
#include <SFML/Graphics.hpp>

class Client;

class Renderer
{
public:
	bool initialize(Client & client);
	void finalize(Client & client);

	void renderHuman(float x, float y);
private:
	sf::RenderWindow * m_window;
};