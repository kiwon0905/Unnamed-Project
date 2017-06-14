#pragma once

#include "Game/Aabb.h"
#include <SFML/System.hpp>
#include <string>
#include <vector>
#include <iostream>

struct MoveResult
{
	sf::Vector2f v;
	int horizontalTile = 0;
	int verticalTile = 0;
};

class Map
{
public:
	bool loadFromFile(const std::string & s);

	const sf::Vector2i & getSize() const;
	const std::string & getTilesetFile() const;
	int getTileSize() const;
	int getTile(int x, int y) const;
	int getTile(float x, float y) const;
	int sweep(const Aabb & aabb, const sf::Vector2f & d, float & time, sf::Vector2i & norm) const;
	bool isGrounded(const Aabb & aabb) const;
private:

	sf::Vector2i m_size;
	std::string m_tilesetFile;
	int m_tileSize;
	std::vector<std::vector<int>> m_data;
};