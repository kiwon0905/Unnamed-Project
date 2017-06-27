#include "SoundPlayer.h"

sf::Sound & SoundPlayer::add(const sf::SoundBuffer & buf)
{
	m_sounds.emplace_back();
	m_sounds.back().setBuffer(buf);
	return m_sounds.back();
}

void SoundPlayer::update()
{
	auto stopped = [](const sf::Sound & s)
	{
		return s.getStatus() == sf::Sound::Stopped;
	};
	m_sounds.remove_if(stopped);
}
