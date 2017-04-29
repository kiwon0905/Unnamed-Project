#pragma once
#include <SFML/Graphics.hpp>
#include <deque>

class Graph : public sf::Drawable
{
public:
	Graph(float min, float max, const sf::Font & font, const std::string & title = "");

	void addSample(float data);
	void setMaxSampleSize(unsigned size);

	void setSize(const sf::Vector2f & v);
	void setPosition(const sf::Vector2f v);
	void draw(sf::RenderTarget & target, sf::RenderStates states) const;

private:
	std::deque<float> m_data;
	unsigned m_maxSampleSize = 100;

	//these are the min, max value for the axis
	float m_minValue;
	float m_maxValue;

	float m_min;
	float m_max;
	float m_avg = 0.f;

	sf::Vector2f m_size = sf::Vector2f(200.f, 200.f);
	sf::Vector2f m_position;

	std::string m_title;
	const sf::Font * m_font;
};
