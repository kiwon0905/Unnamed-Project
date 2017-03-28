#pragma once

#include "Game/Aabb.h"
#include <SFML/System.hpp>
#include <string>
#include <vector>
#include <iostream>
class Map
{
public:
	bool loadFromFile(const std::string & s);

	const sf::Vector2i & getSize() const;
	const std::string & getTilesetFile() const;
	int getTileSize() const;
	int getTile(int x, int y) const;
	sf::Vector2f move(const Aabb<float> & aabb, const sf::Vector2f & dv) const;
	bool isGrounded(const Aabb<float> & aabb) const;
private:

	sf::Vector2i m_size;
	std::string m_tilesetFile;
	int m_tileSize;
	std::vector<std::vector<int>> m_data;
};