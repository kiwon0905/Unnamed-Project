#include "Map.h"

#include <tinyxml2/tinyxml2.h>
#include <iostream>
#include <sstream>

bool Map::loadFromFile(const std::string & s)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(s.c_str());
	
	tinyxml2::XMLElement * element = doc.FirstChildElement("map");
	m_size.x = element->FirstChildElement("width")->IntText();
	m_size.y = element->FirstChildElement("height")->IntText();
	m_tilesetFile = element->FirstChildElement("tileset")->Attribute("file");
	m_tileSize = element->FirstChildElement("tileset")->IntAttribute("size");
	std::string data = element->FirstChildElement("data")->GetText();
	
	std::istringstream ss(data);
	m_data.emplace_back();
	std::string token;
	while (std::getline(ss, token, ','))
	{
		int tile = std::stoi(token);
		if (m_data.back().size() == static_cast<unsigned>(m_size.x))
			m_data.emplace_back();
		m_data.back().push_back(tile);	
	}
	return true;
}

const sf::Vector2i & Map::getSize() const
{
	return m_size;
}

const std::string & Map::getTilesetFile() const
{
	return m_tilesetFile;
}

int Map::getTileSize() const
{
	return m_tileSize;
}

const std::vector<std::vector<int>> Map::getData() const
{
	return m_data;
}
