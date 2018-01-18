#include "Hud.h"
#include "PlayingScreen.h"

#include "Entity/Zombie.h"
#include "Entity/Human.h"
#include "Game/NetObject/NetGameDataControl.h"
#include "Game/NetObject/NetGameDataTdm.h"
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

	//announcer
	if (!m_announcerMsgs.empty())
	{
		sf::Vector2f pos = sf::Vector2f((target.getSize().x - m_announcerMsgs[0].text.getLocalBounds().width) / 2.f, target.getSize().y * 0.2f);
		states.transform.translate(pos);
		target.draw(m_announcerMsgs[0].text, states);
	}

	//character specific
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


	//game time
	sf::Text timeText;
	timeText.setFillColor(sf::Color::Blue);
	timeText.setString(getStringFromTime(static_cast<int>(m_screen->getCurrentRenderTime().asSeconds())));
	timeText.setFont(*m_font);
	timeText.setPosition(static_cast<float>(target.getSize().x / 2.f - timeText.getLocalBounds().width / 2.f), 0.f);
	target.draw(timeText);

	//game mode specific
	std::string mode;
	m_screen->getMap().getProperty("mode", mode);
	if (mode == "control")
	{
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
	else if (mode == "tdm")
	{
		target.setView(target.getDefaultView());
		Snapshot * current = m_screen->m_currentSnap.snapshot;
		const NetGameDataTdm * ngdt = static_cast<const NetGameDataTdm*>(current->getEntity(NetObject::GAME_DATA_TDM, 0));
		std::string str = "Blue: " + std::to_string(ngdt->scoreA) + "\n" + "Red: " + std::to_string(ngdt->scoreB);
		sf::Text text;
		text.setFont(*m_font);
		text.setString(str);
		text.setPosition(static_cast<float>(target.getSize().x / 2.f - text.getLocalBounds().width / 2.f), timeText.getLocalBounds().height + 5);
		target.draw(text);
		//std::cout << "A: " << ngdt->scoreA << "\n";
		//std::cout << "B: " << ngdt->scoreB << "\n";
	}
}

void Hud::setEntity(const Entity * e)
{
	m_entity = e;
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
