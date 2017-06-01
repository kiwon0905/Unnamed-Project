#pragma once

#include "Game/Control.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <unordered_map>

class Client;

class Input
{
public:

	bool initialize(Client & client);
	void finalize(Client & client);
	unsigned getBits();
	NetInput getInput(const sf::RenderTarget & target, const sf::Window & window);
private:
	std::unordered_map<Control, sf::Keyboard::Key> m_binds;
};