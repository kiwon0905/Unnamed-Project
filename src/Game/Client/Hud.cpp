#include "Hud.h"
#include "PlayingScreen.h"

#include "Entity/Zombie.h"
#include "Entity/Human.h"
#include "Game/NetObject/NetGameDataControl.h"
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


	//character info
	if (!m_entity)
		return;
	if (dynamic_cast<const Zombie*>(m_entity))
	{
		const Zombie * z = static_cast<const Zombie*>(m_entity);
		const ZombieCore & core = z->getCore();
		NetZombie netZombie;
		core.write(netZombie);

		sf::Text fuelText;
		fuelText.setFont(*m_font);
		fuelText.setString("Fuel: " + std::to_string(netZombie.fuel));
		fuelText.setOrigin({ fuelText.getLocalBounds().left, fuelText.getLocalBounds().top });
		fuelText.setPosition(static_cast<sf::Vector2f>(target.getSize()) - sf::Vector2f(fuelText.getLocalBounds().width, fuelText.getLocalBounds().height));

		sf::Text boostCooldownText;
		boostCooldownText.setFont(*m_font);
		boostCooldownText.setString("Boost: " + std::to_string(netZombie.boostCooldown));

		//target.draw(fuelText);
		//target.draw(boostCooldownText);
	}


	//game mode specific
	Map & map = m_screen->m_map;

	std::string mode;
	map.getProperty("mode", mode);
	if (mode == "control")
	{
		std::string controlPoint;
		map.getProperty("control point", controlPoint);
		sf::FloatRect rect;
		sscanf_s(&controlPoint[0], "( %f, %f, %f, %f)", &rect.left, &rect.top, &rect.width, &rect.height);

		target.setView(m_screen->m_view);
		//capture area
		sf::RectangleShape rectShape;
		rectShape.setPosition(sf::Vector2f{ rect.left, rect.top });
		rectShape.setSize(sf::Vector2f{ rect.width, rect.height });
		rectShape.setFillColor(sf::Color::Yellow);
		target.draw(rectShape);
		

		target.setView(target.getDefaultView());
		Snapshot * current = m_screen->m_currentSnap.snapshot;
		const NetGameDataControl * ngdc = static_cast<const NetGameDataControl *>(current->getEntity(NetObject::GAME_DATA_CONTROL, 0));
		if (ngdc)
		{
			std::string s;
			s += "Capturing team: " + toString(ngdc->capturingTeam) + "\n";
			s += "Capture progress A: " + std::to_string(ngdc->captureProgressA) + "\n";
			s += "Capture progress B: " + std::to_string(ngdc->captureProgressB) + "\n";
			s += "Controlling team: " + toString(ngdc->controllingTeam) + "\n";
			s += "Control progress A: " + std::to_string(ngdc->controlProgressA) + "\n";
			s += "Control progress B: " + std::to_string(ngdc->controlProgressB) + "\n";

			sf::Text text;
			text.setFillColor(sf::Color::Black);
			text.setFont(*m_font);
			text.setString(s);

			target.draw(text);
		}
	}
}

void Hud::setEntity(const Entity * e)
{
	m_entity = e;
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
