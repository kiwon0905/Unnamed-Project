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

	struct Object
	{
		int gid;
		bool repeatTexture = false;
		float x, y, w, h;
		sf::Sprite sprite;
	};

public:
	bool loadTextures(AssetManager & assets);

	void drawBackground(sf::RenderTarget & target) const;
	void drawTiles(sf::RenderTarget & target) const;

private:
	sf::VertexArray m_tileVertices;
	sf::Texture * m_tileTexture;
	std::map<int, Tile> m_tiles;
	std::vector<Object> m_backgroundObjects;
};
