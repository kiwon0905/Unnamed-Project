#include "Input.h"
#include "Client.h"

#include <bitset>
 
bool Input::initialize(Client & client)
{
	m_controls[Control::MOVE_LEFT] = std::bind(sf::Keyboard::isKeyPressed, sf::Keyboard::A);
	m_controls[Control::MOVE_RIGHT] = std::bind(sf::Keyboard::isKeyPressed, sf::Keyboard::D);
	m_controls[Control::MOVE_UP] = std::bind(sf::Keyboard::isKeyPressed, sf::Keyboard::W);
	m_controls[Control::MOVE_DOWN] = std::bind(sf::Keyboard::isKeyPressed, sf::Keyboard::S);
	m_controls[Control::JUMP] = std::bind(sf::Keyboard::isKeyPressed, sf::Keyboard::Space);
	m_controls[Control::PRIMARY_FIRE] = std::bind(sf::Mouse::isButtonPressed, sf::Mouse::Left);
	return true;
}

void Input::finalize(Client & client)
{
}

NetInput Input::getInput(const sf::RenderTarget & target, const sf::Window & window)
{
	NetInput input;
	input.aimDirection = target.mapPixelToCoords(sf::Mouse::getPosition(window));
	if (window.hasFocus())
	{
		if (m_controls[Control::MOVE_LEFT]())
			input.moveDirection--;
		if (m_controls[Control::MOVE_RIGHT]())
			input.moveDirection++;

		if (m_controls[Control::MOVE_UP]())
			input.vMoveDirection--;
		if (m_controls[Control::MOVE_DOWN]())
			input.vMoveDirection++;


		input.jump = m_controls[Control::JUMP]();
		input.fire = m_controls[Control::PRIMARY_FIRE]();
	}

	return input;
}
