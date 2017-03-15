#include "Renderer.h"
#include "Core/Client/Client.h"
#include <iostream>
bool Renderer::initialize(Client & client)
{
	m_target = &client.getContext().window;
	m_assetManager = &client.getContext().assetManager;
	m_assetManager->get<sf::Font>("arial.ttf");
	view = m_target->getDefaultView();
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
	m_target->draw(text);
}

void Renderer::renderHuman(float x, float y)
{
	sf::RectangleShape r;
	r.setSize({ 70.f, 70.f });
	r.setFillColor(sf::Color::Yellow);
	r.setPosition({ x,y });
	m_target->draw(r);
}

void Renderer::setViewCenter(float x, float y)
{
	view.setCenter(x, y);
	m_target->setView(view);
}

void Renderer::setViewSize(float x, float y)
{
	view.setSize(x, y);
	m_target->setView(view);
}

void Renderer::setDefaultView()
{
	m_target->setView(m_target->getDefaultView());
}
