#pragma once

#include "Game/Control.h"

#include <SFML/Window.hpp>
#include <unordered_map>

class Client;

class Input
{
public:

	bool initialize(Client & client);
	void finalize(Client & client);
	void update();
	unsigned getBits();
private:
	std::unordered_map<Control, sf::Keyboard::Key> m_binds;
	unsigned m_bits;
};