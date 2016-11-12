#include "Input.h"
#include "Client.h"

#include <bitset>

bool Input::initialize(Client & client)
{
	m_binds[JUMP] = sf::Keyboard::Space;
	m_binds[MOVE_LEFT] = sf::Keyboard::Left;
	m_binds[MOVE_RIGHT] = sf::Keyboard::Right;
	return true;
}

void Input::finalize(Client & client)
{
}

void Input::update()
{
	m_inputBits = 0;
	int direction = 0;

	if (sf::Keyboard::isKeyPressed(m_binds[MOVE_LEFT]))
		direction--;

	if (sf::Keyboard::isKeyPressed(m_binds[MOVE_RIGHT]))
		direction++;

	if (sf::Keyboard::isKeyPressed(m_binds[JUMP]))
		m_inputBits |= 1 << JUMP;

	if (direction == -1)
		m_inputBits |= 1 << MOVE_LEFT;
	else if (direction == 1)
		m_inputBits |= 1 << MOVE_RIGHT;
}

sf::Uint8 Input::getBits()
{
	return m_inputBits;
}
