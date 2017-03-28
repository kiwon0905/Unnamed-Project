#include "Input.h"
#include "Client.h"

#include <bitset>
 
bool Input::initialize(Client & client)
{
	m_binds[Control::MOVE_LEFT] = sf::Keyboard::Left;
	m_binds[Control::MOVE_RIGHT] = sf::Keyboard::Right;
	m_binds[Control::MOVE_UP] = sf::Keyboard::Up;
	m_binds[Control::MOVE_DOWN] = sf::Keyboard::Down;
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

	return bits;
}
