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

void Input::update()
{
	m_bits = 0;
	int direction = 0;

	if (sf::Keyboard::isKeyPressed(m_binds[Control::MOVE_LEFT]))
		direction--;

	if (sf::Keyboard::isKeyPressed(m_binds[Control::MOVE_RIGHT]))
		direction++;

	if (sf::Keyboard::isKeyPressed(m_binds[Control::JUMP]))
		m_bits |= 1 << Control::JUMP;

	if (direction == -1)
		m_bits |= 1 << Control::MOVE_LEFT;
	else if (direction == 1)
		m_bits |= 1 << Control::MOVE_RIGHT;
}

std::uint8_t Input::getBits()
{
	return m_bits;
}
