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
	int slashPos = s.find_last_of("/");
	m_name = s.substr(slashPos + 1, s.size() - 5 - slashPos);
	return true;
}

const std::string & Map::getName()
{
	return m_name;
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

int Map::sweep(const Aabb & aabb, const sf::Vector2f & d, float & time, sf::Vector2i & norm) const
{
	//   x0    x1
	//y0
	//
	//y1

	float x0, x1, y0, y1;
	x0 = d.x > 0 ? aabb.x : aabb.x + d.x;
	y0 = d.y > 0 ? aabb.y : aabb.y + d.y;
	x1 = d.x > 0 ? aabb.x + aabb.w + d.x : aabb.x + aabb.w;
	y1 = d.y > 0 ? aabb.y + aabb.h + d.y : aabb.y + aabb.h;

	int startX = static_cast<int>(std::floor(x0 / m_tileSize));
	int endX = static_cast<int>(std::floor(x1 / m_tileSize));
	int startY = static_cast<int>(std::floor(y0 / m_tileSize));
	int endY = static_cast<int>(std::floor(y1 / m_tileSize));

	int minTile = 0;
	float minTime = 1.f;
	sf::Vector2i minNorm;

	for (int x = startX; x <= endX; ++x)
	{
		for (int y = startY; y <= endY; ++y)
		{
			int tile = getTile(x, y);
			if (tile)
			{
				float time;
				sf::Vector2i norm;
				Aabb tileAabb(x * m_tileSize, y * m_tileSize, m_tileSize, m_tileSize);
				if (aabb.sweep(d, tileAabb, time, norm))
				{
					if (time < minTime)
					{
						minTime = time;
						minTile = tile;
						minNorm = norm;
					}
				}
			}
		}
	}
	time = minTime;
	norm = minNorm;
	return minTile;
}

bool Map::slideSweep(const Aabb & aabb, const sf::Vector2f & d, sf::Vector2f & out, sf::Vector2i & norm, sf::Vector2i & tile) const
{
	Aabb aabb2 = aabb;

	bool collided = false;
	out = d;
	norm = sf::Vector2i();
	//right
	if (d.x > 0)
	{
		float minDistance = d.x + 1000.f;

		//int startX = (aabb2.left + aabb2.width) / m_tileSize + 1;
		int startX = aabb.x / m_tileSize;
		int endX = (aabb2.x + aabb2.w + d.x) / m_tileSize;
		int startY = aabb2.y / m_tileSize;
		int endY = (aabb2.y + aabb2.h) / m_tileSize;


		for (int x = startX; x <= endX; ++x)
		{
			for (int y = startY; y <= endY; ++y)
			{
				int tile = getTile(x, y);
				if (tile)
				{
					float distance = m_tileSize * x - (aabb2.x + aabb2.w);
					if (distance < minDistance)
					{
						minDistance = distance;
						out.x = minDistance - 0.1f;
						norm.x = -1;
						collided = true;
					}
				}
			}
		}
	}
	//left
	else if (d.x < 0)
	{
		float maxDistance = d.x - 1000.f;
		int startX = std::floor((aabb2.x + d.x) / m_tileSize);
		//int endX = aabb2.left / m_tileSize - 1;
		int endX = (aabb2.x + aabb2.w) / m_tileSize;
		int startY = aabb2.y / m_tileSize;
		int endY = (aabb2.y + aabb2.h) / m_tileSize;

		for (int x = startX; x <= endX; ++x)
		{
			for (int y = startY; y <= endY; ++y)
			{
				int tile = getTile(x, y);
				if (tile)
				{
					float distance = (x + 1) * m_tileSize - aabb2.x;
					if (distance > maxDistance)
					{
						maxDistance = distance;
						out.x = maxDistance + 0.1f;
						norm.x = 1;
						collided = true;
					}
				}
			}
		}
	}

	aabb2.x += out.x;
	//down
	if (d.y > 0)
	{
		float minDistance = d.y + 1000.f;
		//int startY = (aabb2.top + aabb2.height) / m_tileSize + 1;
		int startY = (aabb2.y) / m_tileSize;
		int endY = (aabb2.y + aabb2.h + d.y) / m_tileSize;
		int startX = aabb2.x / m_tileSize;
		int endX = (aabb2.x + aabb2.w) / m_tileSize;

		for (int y = startY; y <= endY; ++y)
		{
			for (int x = startX; x <= endX; ++x)
			{
				int tile = getTile(x, y);
				if (tile)
				{
					float distance = m_tileSize * y - (aabb2.y + aabb2.h);
					if (distance < minDistance)
					{
						minDistance = distance;
						out.y = minDistance - 0.1f;
						norm.y = -1;
						collided = true;
					}
				}
			}
		}
	}
	//up
	else if (d.y < 0)
	{
		float maxDistance = d.y - 1000.f;
		int startY = std::floor((aabb2.y + d.y) / m_tileSize);
		//int endY = aabb2.top / m_tileSize - 1;
		int endY = (aabb2.y + aabb2.h) / m_tileSize;
		int startX = aabb2.x / m_tileSize;
		int endX = (aabb2.x + aabb2.w) / m_tileSize;
		for (int y = startY; y <= endY; ++y)
		{

			for (int x = startX; x <= endX; ++x)
			{
				int tile = getTile(x, y);
				if (getTile(x, y))
				{
					float distance = (y + 1) * m_tileSize - aabb2.y;
					if (distance > maxDistance)
					{
						maxDistance = distance;
						out.y = maxDistance + 0.1f;
						norm.y = 1;
						collided = true;
					}
				}
			}
		}
	}
	return collided;
}

bool Map::isGrounded(const Aabb & aabb) const
{
	int startX = std::floor((aabb.x)/ m_tileSize);
	int endX = std::floor((aabb.x + aabb.w) / m_tileSize);

	int y = std::floor((aabb.y + aabb.h + 1) / m_tileSize);

	for (int x = startX; x <= endX; ++x)
	{
		if (getTile(x, y) != 0)
		{
			return true;
		}
	}
	return false;
}

int Map::getTile(int x, int y) const
{
	if (0 <= x && x < m_size.x && 0 <= y && y < m_size.y)
		return m_data[y][x];
	return -1;
}

int Map::getTile(float x, float y) const
{
	int xt = std::floor(x / m_tileSize);
	int yt = std::floor(y / m_tileSize);
	return getTile(xt, yt);
}
