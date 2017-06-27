#pragma once
#include <list>
#include <SFML/Audio.hpp>

class SoundPlayer
{
public:

	sf::Sound & add(const sf::SoundBuffer & buf);
	void update();
private:
	std::list<sf::Sound> m_sounds;
};