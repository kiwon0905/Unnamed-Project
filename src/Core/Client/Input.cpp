#include "Input.h"
#include "Client.h"

#include <bitset>
 
bool Input::initialize(Client & client)
{

	m_controls[Control::MOVE_LEFT] = std::bind(&Input::getKeyState, this, sf::Keyboard::A);
	m_controls[Control::MOVE_RIGHT] = std::bind(&Input::getKeyState, this, sf::Keyboard::D);
	m_controls[Control::MOVE_UP] = std::bind(&Input::getKeyState, this, sf::Keyboard::W);
	m_controls[Control::MOVE_DOWN] = std::bind(&Input::getKeyState, this, sf::Keyboard::S);

	m_controls[Control::JUMP] = [this]() {return isActive({ sf::Keyboard::Space }); };
	m_controls[Control::PRIMARY_FIRE] = [this]() {return isActive({}, { sf::Mouse::Left }); };

	addKeyCombination({ sf::Keyboard::LControl, sf::Keyboard::LShift, sf::Keyboard::D });
	addKeyCombination({ sf::Keyboard::Space });
	addKeyCombination({}, { sf::Mouse::Left });
	addKeyCombination({ sf::Keyboard::Return });

	m_hasFocus = client.getWindow().hasFocus();
	return true;
}

void Input::finalize(Client & client)
{
}

NetInput Input::getInput(const sf::RenderTarget & target, const sf::View & view)
{
	NetInput input;
	input.aimDirection = target.mapPixelToCoords(m_mousePosition, view);

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

void Input::addKeyCombination(const std::unordered_set<sf::Keyboard::Key> & keys, const std::unordered_set<sf::Mouse::Button> & buttons)
{
	KeyCombination comb;
	comb.keys = keys;
	comb.buttons = buttons;

	m_keyCombinations.push_back(comb);
}

bool Input::isActive(const std::unordered_set<sf::Keyboard::Key> & keys, const std::unordered_set<sf::Mouse::Button> & buttons)
{
	for (auto & c : m_keyCombinations)
	{
		if (c.keys == keys && c.buttons == buttons)
		{

			bool currentActive = true;
			for (auto key : c.keys)
			{
				currentActive = currentActive && m_activeKeys[key];
			}
			for (auto button : c.buttons)
			{
				currentActive = currentActive && m_activeButtons[button];
			}

			c.current = (currentActive && !c.last);
			c.last = currentActive;
			return c.current;
		}

	}

	return false;
}

void Input::handleEvent(const sf::Event & event)
{

	if (event.type == sf::Event::LostFocus)
	{
		m_hasFocus = false;
	}
	else if (event.type == sf::Event::GainedFocus)
	{
		m_hasFocus = true;
	}

	if (m_hasFocus)
	{
		if (event.type == sf::Event::KeyPressed)
		{
			m_activeKeys[event.key.code] = true;
		}
		else if (event.type == sf::Event::KeyReleased)
		{
			m_activeKeys[event.key.code] = false;
		}
		else if (event.type == sf::Event::MouseButtonPressed)
		{
			m_activeButtons[event.mouseButton.button] = true;
		}
		else if (event.type == sf::Event::MouseButtonReleased)
		{
			m_activeButtons[event.mouseButton.button] = false;
		}
		else if (event.type == sf::Event::MouseMoved)
		{
			m_mousePosition.x = event.mouseMove.x;
			m_mousePosition.y = event.mouseMove.y;
		}
	}

}

bool Input::getKeyState(sf::Keyboard::Key key)
{
	return  m_activeKeys[key];
}
