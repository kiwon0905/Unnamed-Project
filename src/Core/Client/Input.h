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
	void beginFrame();
	void handleEvent(const sf::Event & event);

	NetInput getInput(const sf::RenderTarget & target, const sf::View & view);
	sf::Vector2i getMousePosition() { return m_mousePosition; }

	bool getKeyState_(sf::Keyboard::Key key, bool currentFrame = false);
	bool getButtonState_(sf::Mouse::Button button, bool currentFrame = false);
	
private:
	struct State
	{
		int frame;
		bool pressed = false;
	};
	State m_keyStates[sf::Keyboard::KeyCount];
	State m_buttonStates[sf::Mouse::ButtonCount];

	int m_frame = 0;
	sf::Vector2i m_mousePosition;
	bool m_hasFocus;

	NetInput m_prevInput;


	std::unordered_map<Control, std::function<bool()>> m_controls;
	bool m_activeKeys[sf::Keyboard::KeyCount] = { false };
	bool m_activeButtons[sf::Mouse::ButtonCount] = { false };
};