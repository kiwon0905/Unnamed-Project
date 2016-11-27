#include "Renderer.h"
#include "Core/Client/Client.h"
#include <iostream>
bool Renderer::initialize(Client & client)
{
	m_window = &client.getContext().window;
	return true;
}

void Renderer::finalize(Client & client)
{
}

void Renderer::renderHuman(float x, float y)
{
	sf::RectangleShape r;
	r.setSize({ 100.f, 100.f });
	r.setFillColor(sf::Color::Yellow);
	r.setPosition({ x,y });
	m_window->draw(r);
}
