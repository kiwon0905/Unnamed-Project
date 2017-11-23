#pragma once

#include <SFML/Graphics.hpp>
#include <deque>
class PlayingScreen;
//game time, ability cooldowns, announcer
class Hud : public sf::Drawable
{
public:
	Hud(const sf::Font & font, PlayingScreen & screen);

	void update(float dt);
	void draw(sf::RenderTarget & target, sf::RenderStates states) const;

	void setGameTime(const sf::Time & time);
	void announce(const std::string & s, const sf::Color & fill = sf::Color::White, const sf::Color & outline = sf::Color::Black);
private:
	const sf::Font * m_font;
	PlayingScreen * m_screen;

	sf::Time m_time;

	struct AnnouncerMsg
	{
		sf::Text text;
		float time = 0.f;
	};
	std::deque<AnnouncerMsg> m_announcerMsgs;
};