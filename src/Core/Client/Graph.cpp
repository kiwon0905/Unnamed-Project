#include "Graph.h"

#include <iostream>
Graph::Graph(float min, float max, const sf::Font & font, const std::string & title) :
	m_minValue(min),
	m_maxValue(max),
	m_min(max),
	m_max(min),
	m_font(&font),
	m_title(title)
{

}

void Graph::addSample(float data)
{
	if (m_data.size() >= m_maxSampleSize)
		m_data.pop_front();
	m_data.push_back(data);
	
	if (data > m_max)
	{
		m_max = data;
	//	m_maxValue = m_max;
	}
	else if (data < m_min)
	{
		m_min = data;
//		m_minValue = m_min;
	}
	
	float sum = 0.f;
	for (float f : m_data)
		sum += f;
	if (!m_data.empty())
		m_avg = sum / m_data.size();
}

void Graph::setMaxSampleSize(unsigned size)
{
	m_maxSampleSize = size;
	if (m_data.size() > size)
	{
		int count = m_data.size() - size;
		m_data.erase(m_data.begin(), m_data.begin() + count);

		float sum = 0.f;
		for (float f : m_data)
			sum += f;
		if (!m_data.empty())
			m_avg = sum / m_data.size();
	}
}

void Graph::setSize(const sf::Vector2f & v)
{
	m_size = v;
}

void Graph::setPosition(const sf::Vector2f v)
{
	m_position = v;
}

void Graph::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	//TODO: improve
	states.transform.translate(m_position);

	//plot the points
	sf::VertexArray points(sf::PrimitiveType::LinesStrip, m_data.size());
	for (std::size_t i = 0; i < m_data.size(); ++i)
	{
		points[i].position = sf::Vector2f(static_cast<float>(m_maxSampleSize - m_data.size() + i), m_maxValue - m_data[i]);
		points[i].color = sf::Color::Blue;
	//	std::cout << "max sample size: " << m_maxSampleSize << "data size: " << m_data.size() << "i: " << i << "pos.x: " << points[i].position.x << "\n";
	}

	//title
	sf::Text title;
	title.setCharacterSize(12);
	title.setFont(*m_font);
	title.setString(m_title);
	target.draw(title, states);

	//statistics
	sf::Text min, max, avg;

	min.setCharacterSize(12);
	max.setCharacterSize(12);
	avg.setCharacterSize(12);

	min.setFont(*m_font);
	max.setFont(*m_font);
	avg.setFont(*m_font);

	min.setString("min: " + std::to_string(m_min));
	max.setString("max: " + std::to_string(m_max));
	avg.setString("avg: " + std::to_string(m_avg));

	min.setPosition(sf::Vector2f(0.f, title.getPosition().y + title.getLocalBounds().height + 3.f));
	max.setPosition(sf::Vector2f(0.f, min.getPosition().y + min.getLocalBounds().height + 3.f));
	avg.setPosition(sf::Vector2f(0.f, max.getPosition().y + max.getLocalBounds().height + 3.f));

	target.draw(min, states);
	target.draw(max, states);
	target.draw(avg, states);

	states.transform.translate(sf::Vector2f(0.f, avg.getPosition().y + avg.getLocalBounds().height + 3.f));

	//axis label
	sf::Text minValue, maxValue;
	minValue.setCharacterSize(12);
	maxValue.setCharacterSize(12);

	minValue.setFont(*m_font);
	maxValue.setFont(*m_font);

	minValue.setPosition(sf::Vector2f(0.f, m_size.y));
	maxValue.setPosition(sf::Vector2f(0.f, 0.f));

	minValue.setString(std::to_string(m_minValue));
	maxValue.setString(std::to_string(m_maxValue));


	target.draw(maxValue, states);
	states.transform.translate(sf::Vector2f(0.f, maxValue.getPosition().y + maxValue.getLocalBounds().height + 3.f));
	target.draw(minValue, states);

	//points
	sf::Vector2f scaleFactor;
	scaleFactor.x = m_size.x / m_maxSampleSize;
	scaleFactor.y = m_size.y / (m_maxValue - m_minValue);
	states.transform.scale(scaleFactor);
	target.draw(points, states);

	//axis
	sf::VertexArray axis(sf::PrimitiveType::Lines, 2);
	axis[0].position = sf::Vector2f(0.f, m_maxValue);
	axis[1].position = sf::Vector2f(static_cast<float>(m_maxSampleSize), m_maxValue);
	target.draw(axis, states);


}
