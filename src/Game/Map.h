#pragma once

#include "Game/Aabb.h"
#include <SFML/System.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <tinyxml2/tinyxml2.h>


class Map
{
protected:
	tinyxml2::XMLElement * getElementWithAttribute(tinyxml2::XMLElement * parent, const std::string & name, const std::string & attribute, const std::string & value);
public:
	virtual bool loadFromTmx(const std::string & s);

	const std::string & getName() const;
	const sf::Vector2i & getSize() const;
	sf::Vector2i getTileSize() const;
	sf::Vector2f getWorldSize() const;
	const std::string & getTilesetFile() const;
	bool getProperty(const std::string & property, std::string & value);
	
	
	int getTile(int x, int y) const;
	int getTile(float x, float y) const;
	bool sweepPoints(const std::vector<sf::Vector2f> & points, const sf::Vector2f d, float & time) const;
	bool sweepCharacter(const Aabb & aabb, const sf::Vector2f & d, sf::Vector2f & out, sf::Vector2i & norm, sf::Vector2i & tile) const;
	bool isGrounded(const Aabb & aabb) const;
	bool leftMap(const Aabb & aabb) const;
private:

	tinyxml2::XMLDocument m_document;

	std::string m_name;
	std::string m_mode;
	sf::Vector2i m_size;
	std::string m_tilesetFile;
	sf::Vector2i m_tileSize;
	std::vector<std::vector<int>> m_data;
	std::unordered_map<std::string, std::string> m_properties;
};