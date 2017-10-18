#pragma once

#include "Game/NetInput.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <functional>

#include <unordered_set>

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
	NetInput getInput(const sf::RenderTarget & target, const sf::View & view);

	void addKeyCombination(const std::unordered_set<sf::Keyboard::Key> & keys, const std::unordered_set<sf::Mouse::Button> & buttons = {});
	bool isActive(const std::unordered_set<sf::Keyboard::Key> & keys, const std::unordered_set<sf::Mouse::Button> & buttons = {});
	sf::Vector2i getMousePosition() { return m_mousePosition; }
	bool getKeyState(sf::Keyboard::Key key);

	
	void handleEvent(const sf::Event & event);
private:
	std::unordered_map<Control, std::function<bool()>> m_controls;
	
	
	bool m_activeKeys[sf::Keyboard::KeyCount] = { false };
	bool m_activeButtons[sf::Mouse::ButtonCount] = { false };
	sf::Vector2i m_mousePosition;
	bool m_hasFocus;
	struct KeyCombination
	{
		std::unordered_set<sf::Keyboard::Key> keys;
		std::unordered_set<sf::Mouse::Button> buttons;
		bool last = false;
		bool current = false;
	};

	std::vector<KeyCombination> m_keyCombinations;

};