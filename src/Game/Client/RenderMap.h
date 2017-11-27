#pragma once

#include "Game/Map.h"

#include <SFML/Graphics.hpp>
#include <map>

class RenderMap : public Map
{
public:
	bool loadFromTmx(const std::string & s);
private:
	tinyxml2::XMLDocument m_document;
	sf::VertexArray m_tileVertices;
	std::map<int, std::string> m_tilesets;
};
