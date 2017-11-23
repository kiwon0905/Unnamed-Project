#include "Hud.h"
#include "PlayingScreen.h"
#include "Core/Utility.h"


Hud::Hud(const sf::Font & font, PlayingScreen & screen):
	m_font(&font),
	m_screen(&screen)
{
}

void Hud::update(float dt)
{

	if (!m_announcerMsgs.empty())
	{
		m_announcerMsgs[0].time += dt;
		if (m_announcerMsgs[0].time > 1.f)
		{
			float t = (m_announcerMsgs[0].time - 1.f);
			int alpha = Math::lerp(255, 0, t);
			sf::Color c = m_announcerMsgs[0].text.getFillColor();
			c.a = alpha;
			m_announcerMsgs[0].text.setFillColor(c);
			c = m_announcerMsgs[0].text.getOutlineColor();
			c.a = alpha;
			m_announcerMsgs[0].text.setOutlineColor(c);
		}
		if (m_announcerMsgs[0].time > 2.f)
			m_announcerMsgs.pop_front();
	}
}

void Hud::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	sf::View v = target.getView();
	target.setView(target.getDefaultView());

	//game time
	sf::Text timeText;
	timeText.setFillColor(sf::Color::Blue);
	int totalSeconds = static_cast<int>(m_time.asMicroseconds()) / 1000000;
	timeText.setString(getStringFromTime(totalSeconds));
	timeText.setFont(*m_font);
	timeText.setPosition(static_cast<float>(target.getSize().x / 2.f - timeText.getLocalBounds().width / 2.f), 0.f);
	target.draw(timeText);

	//announcer
	if (!m_announcerMsgs.empty())
	{
		sf::Vector2f pos = sf::Vector2f((target.getSize().x - m_announcerMsgs[0].text.getLocalBounds().width) / 2.f, target.getSize().y * 0.2f);
		states.transform.translate(pos);
		target.draw(m_announcerMsgs[0].text, states);
	}
	target.setView(v);
}

void Hud::setGameTime(const sf::Time & time)
{
	m_time = time;
}

void Hud::announce(const std::string & s, const sf::Color & fill, const sf::Color & outline)
{
	m_announcerMsgs.emplace_back();
	m_announcerMsgs.back().text.setFont(*m_font);
	m_announcerMsgs.back().text.setFillColor(fill);
	m_announcerMsgs.back().text.setOutlineColor(outline);
	//m_announcerMsgs.back().text.setOutlineThickness(3.f); mem leak here
	m_announcerMsgs.back().text.setString(s);
}