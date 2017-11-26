#pragma once

#include "Game/Aabb.h"
#include <SFML/System.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>


class Map
{
public:
	bool loadFromFile(const std::string & s);
	const std::string & getName() const;
	const std::string & getMode() const;
	const sf::Vector2i & getSize() const;
	const std::string & getTilesetFile() const;
	int getTileSize() const;
	bool getProperty(const std::string & property, std::string & value);
	
	
	int getTile(int x, int y) const;
	int getTile(float x, float y) const;
	bool sweepPoints(const std::vector<sf::Vector2f> & points, const sf::Vector2f d, float & time) const;
	//int sweep(const Aabb & aabb, const sf::Vector2f & d, float & time, sf::Vector2i & norm) const;
	bool sweepCharacter(const Aabb & aabb, const sf::Vector2f & d, sf::Vector2f & out, sf::Vector2i & norm, sf::Vector2i & tile) const;
	bool isGrounded(const Aabb & aabb) const;
private:
	std::string m_name;
	std::string m_mode;
	sf::Vector2i m_size;
	std::string m_tilesetFile;
	int m_tileSize;
	std::vector<std::vector<int>> m_data;
	std::unordered_map<std::string, std::string> m_properties;
};