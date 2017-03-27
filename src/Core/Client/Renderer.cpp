#include "Renderer.h"
#include "Core/Client/Client.h"
#include <iostream>
bool Renderer::initialize(Client & client)
{
	m_target = &client.getContext().window;
	m_assetManager = &client.getContext().assetManager;
	m_assetManager->get<sf::Font>("arial.ttf");
	return true;
}

void Renderer::finalize(Client & client)
{
}

void Renderer::setTarget(sf::RenderTarget * target)
{
	m_target = target;
}

void Renderer::renderText(const std::string & s, float x, float y, unsigned size, sf::Color color)
{
	sf::Font * font = m_assetManager->get<sf::Font>("arial.ttf");
	sf::Text text;
	text.setFillColor(sf::Color::White);
	text.setString(s);
	text.setFont(*font);
	text.setPosition(static_cast<int>(x + .5f), static_cast<int>(y + .5f));
	text.setCharacterSize(size);
	text.setFillColor(color);
	m_target->draw(text);
}

void Renderer::renderHuman(float x, float y)
{
	sf::RectangleShape r;
	r.setSize({ 50.f, 50.f });
	r.setFillColor(sf::Color::Yellow);
	r.setPosition({ x, y });
	std::string pos = "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
	renderText(pos, x, y - 15.f, 12, sf::Color::Black);
	m_target->draw(r);
}