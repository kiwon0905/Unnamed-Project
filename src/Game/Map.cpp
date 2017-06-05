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
	
	std::string firstLine;
	std::getline(ss, firstLine);
	for(int r = 0; r < m_size.y; ++r)
	{
		std::string line;
		std::getline(ss, line);
		std::istringstream liness(line);

		m_data.emplace_back();
		std::string token;
		while (std::getline(liness, token, ','))
		{
			int tile = std::stoi(token);
			m_data.back().emplace_back(tile);
		}
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

MoveResult Map::move(const Aabb<float> & aabb, const sf::Vector2f & dv) const
{
	MoveResult result;
	result.v = dv;

	Aabb<float> aabb2 = aabb;

	//right
	if (dv.x > 0)
	{
		float minDistance = dv.x + 1000.f;

		//int startX = (aabb2.left + aabb2.width) / m_tileSize + 1;
		int startX = aabb.left / m_tileSize;
		int endX = (aabb2.left + aabb2.width + dv.x) / m_tileSize;
		int startY = aabb2.top / m_tileSize;
		int endY = (aabb2.top + aabb2.height) / m_tileSize;


		for(int x = startX; x <= endX; ++x)
		{
			for (int y = startY; y <= endY; ++y)
			{
				int tile = getTile(x, y);
				if (tile)
				{
					float distance = m_tileSize * x - (aabb2.left + aabb2.width);
					if (distance < minDistance)
					{
						minDistance = distance;
						result.v.x = minDistance - 0.1f;
						result.horizontalTile = tile;
					}
				}
			}
		}
	}
	//left
	else if (dv.x < 0)
	{
		float maxDistance = dv.x - 1000.f;
		int startX = std::floor((aabb2.left + dv.x) / m_tileSize);
		//int endX = aabb2.left / m_tileSize - 1;
		int endX = (aabb2.left + aabb2.width) / m_tileSize;
		int startY = aabb2.top / m_tileSize;
		int endY = (aabb2.top + aabb2.height) / m_tileSize;

		for (int x = startX; x <= endX; ++x)
		{
			for (int y = startY; y <= endY; ++y)
			{
				int tile = getTile(x, y);
				if (tile)
				{
					float distance = (x + 1) * m_tileSize - aabb2.left;
					if (distance > maxDistance)
					{
						maxDistance = distance;
						result.v.x = maxDistance + 0.1f;
						result.horizontalTile = tile;
					}
				}
			}
		}
	}

	aabb2.left += result.v.x;
	//down
	if (dv.y > 0)
	{
		float minDistance = dv.y + 1000.f;
		//int startY = (aabb2.top + aabb2.height) / m_tileSize + 1;
		int startY = (aabb2.top) / m_tileSize;
		int endY = (aabb2.top + aabb2.height + dv.y) / m_tileSize;
		int startX = aabb2.left / m_tileSize;
		int endX = (aabb2.left + aabb2.width) / m_tileSize;
		
		for(int y = startY; y <= endY; ++y)
		{
			for (int x = startX; x <= endX; ++x)
			{
				int tile = getTile(x, y);
				if (tile)
				{
					float distance = m_tileSize * y - (aabb2.top + aabb2.height);
					if (distance < minDistance)
					{
						minDistance = distance;
						result.v.y = minDistance - 0.1f;
						result.verticalTile = tile;
					}
				}
			}
		}
	}
	//up
	else if (dv.y < 0)
	{
		float maxDistance = dv.y - 1000.f;
		int startY = std::floor((aabb2.top + dv.y) / m_tileSize);	
		//int endY = aabb2.top / m_tileSize - 1;
		int endY = (aabb2.top + aabb2.height) / m_tileSize;
		int startX = aabb2.left / m_tileSize;
		int endX = (aabb2.left + aabb2.width) / m_tileSize;
		for(int y = startY; y <= endY; ++y)
		{

			for (int x = startX; x <= endX; ++x)
			{
				int tile = getTile(x, y);
				if (getTile(x, y))
				{
					float distance = (y + 1) * m_tileSize - aabb2.top;
					if (distance > maxDistance)
					{
						maxDistance = distance;
						result.v.y = maxDistance + 0.1f;
						result.verticalTile = tile;
					}
				}
			}
		}
	}
	return result;
}

bool Map::isGrounded(const Aabb<float> & aabb) const
{
	int startX = aabb.left / m_tileSize;
	int endX = (aabb.left + aabb.width) / m_tileSize;

	int y = (aabb.top + aabb.height + 1) / m_tileSize;

	for (int x = startX; x <= endX; ++x)
	{
		if (getTile(x, y) != 0)
			return true;
	}
	return false;
}

int Map::getTile(int x, int y) const
{
	if (0 <= x && x < m_size.x && 0 <= y && y < m_size.y)
		return m_data[y][x];
	return -1;
}
