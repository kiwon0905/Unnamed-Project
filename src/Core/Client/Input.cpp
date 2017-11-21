#include "Input.h"
#include "Client.h"

#include <bitset>
 
bool Input::initialize(Client & client)
{

	m_controls[Control::MOVE_LEFT] = std::bind(&Input::getKeyState, this, sf::Keyboard::A, false);
	m_controls[Control::MOVE_RIGHT] = std::bind(&Input::getKeyState, this, sf::Keyboard::D, false);
	m_controls[Control::MOVE_UP] = std::bind(&Input::getKeyState, this, sf::Keyboard::W, false);
	m_controls[Control::MOVE_DOWN] = std::bind(&Input::getKeyState, this, sf::Keyboard::S, false);

	m_controls[Control::JUMP] = std::bind(&Input::getKeyState, this, sf::Keyboard::Space, false);
	m_controls[Control::PRIMARY_FIRE] = std::bind(&Input::getButtonState, this, sf::Mouse::Left, false);

	m_hasFocus = client.getWindow().hasFocus();

	m_prevInput.jump = false;
	m_prevInput.fire = false;
	return true;
}

void Input::finalize(Client & client)
{
}

void Input::beginFrame()
{
	m_frame++;
}

NetInput Input::getInput(const sf::RenderTarget & target, const sf::View & view)
{
	NetInput rawInput;
	rawInput.aimDirection = target.mapPixelToCoords(m_mousePosition, view);

	if (m_controls[Control::MOVE_LEFT]())
		rawInput.moveDirection--;
	if (m_controls[Control::MOVE_RIGHT]())
		rawInput.moveDirection++;


	rawInput.jump = m_controls[Control::JUMP]();
	rawInput.fire = m_controls[Control::PRIMARY_FIRE]();


	NetInput filteredInput = rawInput;
	if (m_prevInput.jump)
		filteredInput.jump = false;
	m_prevInput = rawInput;


	return filteredInput;

}


void Input::handleEvent(const sf::Event & event)
{
	if (event.type == sf::Event::LostFocus)
	{
		m_hasFocus = false;
		for (auto & k : m_keyStates)
			k.pressed = false;
		for (auto & b : m_buttonStates)
			b.pressed = false;
	}
	else if (event.type == sf::Event::GainedFocus)
	{
		m_hasFocus = true;
	}

	if (m_hasFocus)
	{
		if (event.type == sf::Event::KeyPressed)
		{
			State & s = m_keyStates[event.key.code];
			if (!s.pressed)
			{
				s.pressed = true;
				s.frame = m_frame;
			}
		}
		else if (event.type == sf::Event::KeyReleased)
		{
			m_keyStates[event.key.code].pressed = false;
		}
		else if (event.type == sf::Event::MouseButtonPressed)
		{
			State & s = m_buttonStates[event.mouseButton.button];
			if (!s.pressed)
			{
				s.pressed = true;
				s.frame = m_frame;
			}
		}
		else if (event.type == sf::Event::MouseButtonReleased)
		{
			m_buttonStates[event.mouseButton.button].pressed = false;
		}
		else if (event.type == sf::Event::MouseMoved)
		{
			m_mousePosition.x = event.mouseMove.x;
			m_mousePosition.y = event.mouseMove.y;
		}
	}
}

bool Input::getKeyState(sf::Keyboard::Key key, bool currentFrame)
{
	if (currentFrame)
		return m_keyStates[key].pressed && m_keyStates[key].frame == m_frame;

	return m_keyStates[key].pressed;
}

bool Input::getButtonState(sf::Mouse::Button button, bool currentFrame)
{
	if (currentFrame)
		return m_buttonStates[button].pressed && m_buttonStates[button].frame == m_frame;
	return m_buttonStates[button].pressed;
}
