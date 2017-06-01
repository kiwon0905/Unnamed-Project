#include "Input.h"
#include "Client.h"

#include <bitset>
 
bool Input::initialize(Client & client)
{
	m_binds[Control::MOVE_LEFT] = sf::Keyboard::Left;
	m_binds[Control::MOVE_RIGHT] = sf::Keyboard::Right;
	m_binds[Control::MOVE_UP] = sf::Keyboard::Up;
	m_binds[Control::MOVE_DOWN] = sf::Keyboard::Down;
	m_binds[Control::PRIMARY_FIRE] = sf::Keyboard::LControl;
	m_binds[Control::JUMP] = sf::Keyboard::Space;
	return true;
}

void Input::finalize(Client & client)
{
}


unsigned Input::getBits()
{
	unsigned bits = 0;

	if (sf::Keyboard::isKeyPressed(m_binds[Control::MOVE_LEFT]))
		bits |= Control::MOVE_LEFT;

	if (sf::Keyboard::isKeyPressed(m_binds[Control::MOVE_RIGHT]))
		bits |= Control::MOVE_RIGHT;

	if (sf::Keyboard::isKeyPressed(m_binds[Control::MOVE_UP]))
		bits |= Control::MOVE_UP;

	if (sf::Keyboard::isKeyPressed(m_binds[Control::MOVE_DOWN]))
		bits |= Control::MOVE_DOWN;

	if (sf::Keyboard::isKeyPressed(m_binds[Control::JUMP]))
		bits |= Control::JUMP;

	if (sf::Keyboard::isKeyPressed(m_binds[Control::PRIMARY_FIRE]))
		bits |= Control::PRIMARY_FIRE;

	return bits;
}

NetInput Input::getInput(const sf::RenderTarget & target, const sf::Window & window)
{
	NetInput input;
	input.aimDirection = target.mapPixelToCoords(sf::Mouse::getPosition(window));
	input.moveDirection = 0;

	if (sf::Keyboard::isKeyPressed(m_binds[Control::MOVE_LEFT]))
		input.moveDirection--;
	if (sf::Keyboard::isKeyPressed(m_binds[Control::MOVE_RIGHT]))
		input.moveDirection++;

	input.jump = sf::Keyboard::isKeyPressed(m_binds[Control::JUMP]);
	input.fire = sf::Keyboard::isKeyPressed(m_binds[Control::PRIMARY_FIRE]);

	input.aimDirection = target.mapPixelToCoords(sf::Mouse::getPosition(window));
	

	return input;
}
