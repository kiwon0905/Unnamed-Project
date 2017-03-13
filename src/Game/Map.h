#pragma once
#include <SFML/System.hpp>
#include <string>
#include <vector>
class Map
{
public:
	bool loadFromFile(const std::string & s);

	const sf::Vector2i & getSize() const;
	const std::string & getTilesetFile() const;
	int getTileSize() const;
	const std::vector<std::vector<int>> getData() const;
private:
	sf::Vector2i m_size;
	std::string m_tilesetFile;
	int m_tileSize;
	std::vector<std::vector<int>> m_data;
};