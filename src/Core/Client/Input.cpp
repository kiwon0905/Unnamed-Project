#include "Input.h"
#include "Client.h"

#include <bitset>

bool Input::initialize(Client & client)
{
	m_binds[Control::JUMP] = sf::Keyboard::Space;
	m_binds[Control::MOVE_LEFT] = sf::Keyboard::Left;
	m_binds[Control::MOVE_RIGHT] = sf::Keyboard::Right;
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

	if (sf::Keyboard::isKeyPressed(m_binds[Control::JUMP]))
		m_bits |= Control::JUMP;

	return m_bits;
}
