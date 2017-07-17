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

	addKeyCombination({ sf::Keyboard::LControl, sf::Keyboard::LShift, sf::Keyboard::D });
	return true;
}

void Input::finalize(Client & client)
{
}

NetInput Input::getInput(const sf::RenderTarget & target, const sf::Window & window)
{
	static NetInput prevRawInput;
	NetInput currentRawInput;
	currentRawInput.aimDirection = target.mapPixelToCoords(sf::Mouse::getPosition(window));
	if (window.hasFocus())
	{
		if (m_controls[Control::MOVE_LEFT]())
			currentRawInput.moveDirection--;
		if (m_controls[Control::MOVE_RIGHT]())
			currentRawInput.moveDirection++;

		if (m_controls[Control::MOVE_UP]())
			currentRawInput.vMoveDirection--;
		if (m_controls[Control::MOVE_DOWN]())
			currentRawInput.vMoveDirection++;


		currentRawInput.jump = m_controls[Control::JUMP]();
		currentRawInput.fire = m_controls[Control::PRIMARY_FIRE]();
	}
	NetInput sendInput = currentRawInput;
	sendInput.fire = sendInput.jump = false;
	if (currentRawInput.jump && !prevRawInput.jump)
		sendInput.jump = true;
	if (currentRawInput.fire && !prevRawInput.fire)
		sendInput.fire = true;
	prevRawInput = currentRawInput;
	return sendInput;
}

void Input::addKeyCombination(std::vector<sf::Keyboard::Key> keys)
{
	KeyCombination comb;
	comb.keys = keys;

	m_keyCombinations.push_back(comb);
}

bool Input::isActive(std::vector<sf::Keyboard::Key> keys)
{
	for (const auto & c : m_keyCombinations)
		if (c.keys == keys)
			return c.current;

	return false;
}

void Input::update()
{
	for (auto & c : m_keyCombinations)
	{
		bool currentActive = sf::Keyboard::isKeyPressed(c.keys[0]);
	
		for (auto key : c.keys)
		{
			currentActive = currentActive && sf::Keyboard::isKeyPressed(key);
		}

		c.current = (currentActive && !c.last);
		c.last = currentActive;

	}
}
