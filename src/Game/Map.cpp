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
	
	std::vector<int> fill(m_size.x + 2, -1);
	m_data.emplace_back(fill);

	std::string firstLine;
	std::getline(ss, firstLine);
	for(int r = 0; r < m_size.y; ++r)
	{
		std::string line;
		std::getline(ss, line);
		std::istringstream liness(line);

		m_data.emplace_back();
		m_data.back().emplace_back(-1);
		std::string token;
		while (std::getline(liness, token, ','))
		{
			int tile = std::stoi(token);
			m_data.back().emplace_back(tile);
		}
		m_data.back().emplace_back(-1);
	}

	m_data.emplace_back(fill);
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

	if (dv.x > 0)
	{
		int x = (aabb2.left + aabb2.width) / m_tileSize + 1;
		float minDistance = dv.x;
		
		while (x < m_size.x + 1 && x * m_tileSize < aabb2.left + aabb2.width + dv.x)
		{
			int startY = aabb2.top / m_tileSize;
			int endY = (aabb2.top + aabb2.height) / m_tileSize;
			for (int y = startY; y <= endY; ++y)
			{
				if (getTile(x, y))
				{
					float distance = m_tileSize * x - (aabb2.left + aabb2.width);
					if (distance < minDistance)
					{
						minDistance = distance;
						result.v.x = minDistance - 0.001f;
						result.horizontalTile = getTile(x, y);
					}
				}
			}
			x++;
		}
	}
	else if (dv.x < 0)
	{
		int x = (aabb2.left) / m_tileSize - 1;
		float maxDistance = dv.x;
		while (x >= -1 && (x + 1) * m_tileSize > aabb2.left + dv.x)
		{
			int startY = aabb2.top / m_tileSize;
			int endY = (aabb2.top + aabb2.height) / m_tileSize;
			for (int y = startY; y <= endY; ++y)
			{
				if (getTile(x, y))
				{
					float distance = (x + 1) * m_tileSize - aabb2.left;
					if (distance > maxDistance)
					{
						maxDistance = distance;
						result.v.x = maxDistance + 0.001f;
						result.horizontalTile = getTile(x, y);
					}
				}
			}
			--x;
		}
	}

	aabb2.left += result.v.x;
	if (dv.y > 0)
	{
		int y = (aabb2.top + aabb2.height) / m_tileSize + 1;
		float minDistance = dv.y;
		while (y < m_size.y + 1 && y * m_tileSize < aabb2.top + aabb2.height + dv.y)
		{
			int startX = aabb2.left / m_tileSize;
			int endX = (aabb2.left + aabb2.width) / m_tileSize;
			for (int x = startX; x <= endX; ++x)
			{
				if (getTile(x, y))
				{
					float distance = m_tileSize * y - (aabb2.top + aabb2.height);
					if (distance < minDistance)
					{
						minDistance = distance;
						result.v.y = minDistance - 0.001f;
						result.verticalTile = getTile(x, y);
					}
				}
			}
			++y;
		}
	}
	else if (dv.y < 0)
	{
		int y = aabb2.top / m_tileSize - 1;
		float maxDistance = dv.y;
		while (y >= -1 && (y + 1) * m_tileSize > aabb2.top + dv.y)
		{
			int startX = aabb2.left / m_tileSize;
			int endX = (aabb2.left + aabb2.width) / m_tileSize;
			for (int x = startX; x <= endX; ++x)
			{
				if (getTile(x, y))
				{
					float distance = (y + 1) * m_tileSize - aabb2.top;
					if (distance > maxDistance)
					{
						maxDistance = distance;
						result.v.y = maxDistance + 0.001f;
						result.verticalTile = getTile(x, y);
					}
				}
			}
			--y;
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
		if (getTile(x, y) > 0)
			return true;
	}
	return false;
}

int Map::getTile(int x, int y) const
{
	if (0 <= x && x < m_size.x && 0 <= y && y < m_size.y)
		return m_data[y + 1][x + 1];
	return -1;
}
