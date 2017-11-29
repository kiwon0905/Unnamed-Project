#include "Map.h"

#include <iostream>
#include <sstream>

tinyxml2::XMLElement * Map::getElementWithAttribute(tinyxml2::XMLElement * parent, const std::string & name, const std::string & attribute, const std::string & value)
{
	for (tinyxml2::XMLElement * ele = parent->FirstChildElement(name.c_str()); ele != nullptr; ele = ele->NextSiblingElement(name.c_str()))
	{
		if (ele->Attribute(attribute.c_str(), value.c_str()))
		{
			return ele;
		}
	}
	return nullptr;
}

//TODO: add validation checks
bool Map::loadFromTmx(const std::string & s)
{
	using namespace tinyxml2;
	int slashPos = s.find_last_of("/");
	m_name = s.substr(slashPos + 1, s.size() - 5 - slashPos);

	if (m_document.LoadFile(s.c_str()) != 0)
		return false;

	//metadata
	XMLElement * element = m_document.FirstChildElement("map");
	m_size.x = element->IntAttribute("width");
	m_size.y = element->IntAttribute("height");
	m_tileSize.x = element->IntAttribute("tilewidth");
	m_tileSize.y = element->IntAttribute("tileheight");

	//properties
	XMLElement * properties = element->FirstChildElement("properties");
	for (tinyxml2::XMLElement * property = properties->FirstChildElement("property"); property != nullptr; property = property->NextSiblingElement("property"))
	{
		m_properties[std::string(property->Attribute("name"))] = std::string(property->Attribute("value"));
	}

	XMLElement * collisionLayer = getElementWithAttribute(element, "layer", "name", "Collision Layer");
	std::string data = collisionLayer->FirstChildElement("data")->GetText();
	
	std::istringstream ss(data);
	std::string firstLine;
	std::getline(ss, firstLine);
	for (int r = 0; r < m_size.y; ++r)
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

const std::string & Map::getName() const
{
	return m_name;
}

const sf::Vector2i & Map::getSize() const
{
	return m_size;
}

sf::Vector2i Map::getTileSize() const
{
	return m_tileSize;
}

sf::Vector2f Map::getWorldSize() const
{
	return sf::Vector2f(static_cast<float>(m_size.x * m_tileSize.x), static_cast<float>(m_size.y * m_tileSize.y));
}

bool Map::sweepPoints(const std::vector<sf::Vector2f>& points, const sf::Vector2f d, float & time) const
{
	bool collided = false;
	float minTime = std::numeric_limits<float>::max();
	for (const auto & p : points)
	{
		float minX = std::min(p.x, p.x + d.x);
		float maxX = std::max(p.x, p.x + d.x);
		float minY = std::min(p.y, p.y + d.y);
		float maxY = std::max(p.y, p.y + d.y);

		int startX = static_cast<int>(std::floor(minX / m_tileSize.x));
		int endX = static_cast<int>(std::floor(maxX / m_tileSize.x));
		int startY = static_cast<int>(std::floor(minY / m_tileSize.y));
		int endY = static_cast<int>(std::floor(maxY / m_tileSize.y));


		for (int x = startX; x <= endX; ++x)
		{
			for (int y = startY; y <= endY; ++y)
			{
				int tile = getTile(x, y);
				Aabb tileAabb(static_cast<float>(x * m_tileSize.x), static_cast<float>(y * m_tileSize.y), static_cast<float>(m_tileSize.x), static_cast<float>(m_tileSize.y));

				if (tile)
				{
					float t;
					if (tileAabb.testLine(p, p + d, t))
					{
						collided = true;
						if (t < minTime)
						{
							minTime = t;
						}
					}
				}
			}
		}
	}
	if (collided)
		time = minTime;
	return collided;
}

bool Map::sweepCharacter(const Aabb & aabb, const sf::Vector2f & d, sf::Vector2f & out, sf::Vector2i & norm, sf::Vector2i & tile) const
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
		int startX = static_cast<int>(aabb.x / m_tileSize.x);
		int endX = static_cast<int>((aabb2.x + aabb2.w + d.x) / m_tileSize.x);
		int startY = static_cast<int>(aabb2.y / m_tileSize.y);
		int endY = static_cast<int>((aabb2.y + aabb2.h) / m_tileSize.y);


		for (int x = startX; x <= endX; ++x)
		{
			for (int y = startY; y <= endY; ++y)
			{
				int tile = getTile(x, y);
				if (tile > 0)
				{
					float distance = m_tileSize.x * x - (aabb2.x + aabb2.w);
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
		int startX = static_cast<int>(std::floor((aabb2.x + d.x) / m_tileSize.x));
		int endX = static_cast<int>((aabb2.x + aabb2.w) / m_tileSize.x);
		int startY = static_cast<int>(aabb2.y / m_tileSize.y);
		int endY = static_cast<int>((aabb2.y + aabb2.h) / m_tileSize.y);

		for (int x = startX; x <= endX; ++x)
		{
			for (int y = startY; y <= endY; ++y)
			{
				int tile = getTile(x, y);
				if (tile > 0)
				{
					float distance = (x + 1) * m_tileSize.x - aabb2.x;
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
		int startY = static_cast<int>((aabb2.y) / m_tileSize.y);
		int endY = static_cast<int>((aabb2.y + aabb2.h + d.y) / m_tileSize.y);
		int startX = static_cast<int>(aabb2.x / m_tileSize.x);
		int endX = static_cast<int>((aabb2.x + aabb2.w) / m_tileSize.x);

		for (int y = startY; y <= endY; ++y)
		{
			for (int x = startX; x <= endX; ++x)
			{
				int tile = getTile(x, y);
				if (tile > 0)
				{
					float distance = m_tileSize.y * y - (aabb2.y + aabb2.h);
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
		int startY = static_cast<int>(std::floor((aabb2.y + d.y) / m_tileSize.y));
		int endY = static_cast<int>((aabb2.y + aabb2.h) / m_tileSize.y);
		int startX = static_cast<int>(aabb2.x / m_tileSize.x);
		int endX = static_cast<int>((aabb2.x + aabb2.w) / m_tileSize.x);
		for (int y = startY; y <= endY; ++y)
		{

			for (int x = startX; x <= endX; ++x)
			{
				int tile = getTile(x, y);
				if (tile > 0)
				{
					float distance = (y + 1) * m_tileSize.y - aabb2.y;
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
	int startX = static_cast<int>(std::floor((aabb.x)/ m_tileSize.x));
	int endX = static_cast<int>(std::floor((aabb.x + aabb.w) / m_tileSize.x));

	int y = static_cast<int>(std::floor((aabb.y + aabb.h + 1) / m_tileSize.y));

	for (int x = startX; x <= endX; ++x)
	{
		if (getTile(x, y) != 0)
		{
			return true;
		}
	}
	return false;
}

bool Map::leftMap(const Aabb & aabb) const
{
	Aabb mapAabb{ 0.f, 0.f, static_cast<float>(m_tileSize.x * m_size.x), static_cast<float>(m_tileSize.y * m_size.y) };
	return !mapAabb.intersects(aabb);
}

int Map::getTile(int x, int y) const
{
	if (0 <= x && x < m_size.x && 0 <= y && y < m_size.y)
		return m_data[y][x];
	return -1;
}

int Map::getTile(float x, float y) const
{
	int xt = static_cast<int>(std::floor(x / m_tileSize.x));
	int yt = static_cast<int>(std::floor(y / m_tileSize.y));
	return getTile(xt, yt);
}

bool Map::getProperty(const std::string & property, std::string & value)
{
	if(m_properties.count(property) == 0)
		return false;
	value = m_properties[property];
	return true;
}
