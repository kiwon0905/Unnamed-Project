#include "Renderer.h"
#include "Core/Client/Client.h"
#include <iostream>
bool Renderer::initialize(Client & client)
{
	m_window = &client.getContext().window;
	m_assetManager = &client.getContext().assetManager;
	m_assetManager->get<sf::Font>("arial.ttf");
	return true;
}

void Renderer::finalize(Client & client)
{
}

void Renderer::renderText(const std::string & s, float x, float y)
{
	sf::Font * font = m_assetManager->get<sf::Font>("arial.ttf");
	sf::Text text;
	text.setFillColor(sf::Color::White);
	text.setString(s);
	text.setFont(*font);
	text.setPosition(x, y);
	m_window->draw(text);
}

void Renderer::renderHuman(float x, float y)
{
	sf::RectangleShape r;
	r.setSize({ 100.f, 100.f });
	r.setFillColor(sf::Color::Yellow);
	r.setPosition({ x,y });
	m_window->draw(r);
}
