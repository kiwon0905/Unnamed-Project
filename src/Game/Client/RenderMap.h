#pragma once

#include "Game/Map.h"
#include "Core/Client/AssetManager.h"
#include <map>


struct Tile
{
	sf::IntRect textureRect;
	std::string source;
};


class RenderMap : public Map
{
	struct ObjectLayer
	{
		float parallaxFactor = 1.f;
		std::vector<sf::Sprite> objects;
	};
public:
	bool loadTextures(AssetManager & assets);

	void drawBackground(sf::RenderTarget & target) const;
	void drawForeground(sf::RenderTarget & target) const;
	void drawTiles(sf::RenderTarget & target) const;

private:
	sf::VertexArray m_tileVertices;
	sf::Texture * m_tileTexture;
	std::map<int, Tile> m_tiles;

	std::vector<ObjectLayer> m_backgroundLayers;
	std::vector<sf::Sprite> m_foregroundLayer;
};
