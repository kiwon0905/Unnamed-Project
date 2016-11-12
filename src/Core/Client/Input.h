#pragma once

#include <SFML/Window.hpp>
#include <unordered_map>

class Client;

class Input
{
public:
	enum Control
	{
		MOVE_LEFT,
		MOVE_RIGHT,
		JUMP
	};

	bool initialize(Client & client);
	void finalize(Client & client);
	void update();
	sf::Uint8 getBits();
private:
	std::unordered_map<Control, sf::Keyboard::Key> m_binds;
	sf::Uint8 m_inputBits;
};