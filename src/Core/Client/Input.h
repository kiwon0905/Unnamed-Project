#pragma once

#include "Game/NetInput.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <unordered_map>

enum Control
{
	JUMP = 1 << 0,
	MOVE_LEFT = 1 << 1,
	MOVE_RIGHT = 1 << 2,
	MOVE_UP = 1 << 3,
	MOVE_DOWN = 1 << 4,
	PRIMARY_FIRE = 1 << 5,
	ABILITY1 = 1 << 6,
	ABILITY2 = 1 << 7,
};

class Client;

class Input
{
public:

	bool initialize(Client & client);
	void finalize(Client & client);
	NetInput getInput(const sf::RenderTarget & target, const sf::Window & window);
private:
	std::unordered_map<Control, sf::Keyboard::Key> m_binds;
};