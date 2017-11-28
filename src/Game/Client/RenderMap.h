#pragma once

#include "Game/Map.h"
#include "Core/Client/AssetManager.h"
#include <SFML/Graphics.hpp>
#include <map>


struct Tile
{
	sf::IntRect textureRect;
	std::string source;
};

class RenderMap : public Map
{
public:
	bool loadTextures(AssetManager & assets);

	void drawBackground(sf::RenderTarget & target) const;
	void drawTiles(sf::RenderTarget & target) const;
private:
	sf::VertexArray m_tileVertices;
	std::map<int, Tile> m_tiles;
	sf::Texture * m_tileTexture;
};
