#include "Input.h"
#include "Client.h"

#include <bitset>
 
bool Input::initialize(Client & client)
{
	m_binds[Control::MOVE_LEFT] = sf::Keyboard::A;
	m_binds[Control::MOVE_RIGHT] = sf::Keyboard::D;
	m_binds[Control::MOVE_UP] = sf::Keyboard::W;
	m_binds[Control::MOVE_DOWN] = sf::Keyboard::S;
	m_binds[Control::JUMP] = sf::Keyboard::Space;
	return true;
}

void Input::finalize(Client & client)
{
}

NetInput Input::getInput(const sf::RenderTarget & target, const sf::Window & window)
{
	NetInput input;

	if (window.hasFocus())
	{
		input.aimDirection = target.mapPixelToCoords(sf::Mouse::getPosition(window));
		input.moveDirection = 0;

		if (sf::Keyboard::isKeyPressed(m_binds[Control::MOVE_LEFT]))
			input.moveDirection--;
		if (sf::Keyboard::isKeyPressed(m_binds[Control::MOVE_RIGHT]))
			input.moveDirection++;

		input.jump = sf::Keyboard::isKeyPressed(m_binds[Control::JUMP]);
		input.fire = sf::Mouse::isButtonPressed(sf::Mouse::Left);

		input.aimDirection = target.mapPixelToCoords(sf::Mouse::getPosition(window));
	}

	return input;
}
