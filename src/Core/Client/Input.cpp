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
	m_bits = 0;

	if (sf::Keyboard::isKeyPressed(m_binds[Control::MOVE_LEFT]))
		m_bits |= Control::MOVE_LEFT;

	if (sf::Keyboard::isKeyPressed(m_binds[Control::MOVE_RIGHT]))
		m_bits |= Control::MOVE_RIGHT;


	if (sf::Keyboard::isKeyPressed(m_binds[Control::MOVE_UP]))
		m_bits |= Control::MOVE_UP;

	if (sf::Keyboard::isKeyPressed(m_binds[Control::MOVE_DOWN]))
		m_bits |= Control::MOVE_DOWN;


	if (sf::Keyboard::isKeyPressed(m_binds[Control::JUMP]))
		m_bits |= Control::JUMP;

	return m_bits;
}
