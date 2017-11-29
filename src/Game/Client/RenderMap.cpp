#include "RenderMap.h"


//need to improve this

bool RenderMap::loadTextures(AssetManager & assets)
{
	using namespace tinyxml2;
	XMLElement * map = m_document.FirstChildElement("map");

	//load tile infos
	for (XMLElement * tileset = map->FirstChildElement("tileset"); tileset != nullptr; tileset = tileset->NextSiblingElement("tileset"))
	{
		int firstGid = -1;
		std::string source;
		tileset->QueryIntAttribute("firstgid", &firstGid);
		source = tileset->Attribute("source");
		
		//parse the .tsx file

		XMLDocument tsx;
		tsx.LoadFile(source.c_str());

		std::cout << "source: " << source << "\n";
		XMLElement * tst = tsx.FirstChildElement("tileset");

		int tileCount;
		int tileWidth;
		int tileHeight;
		
		tst->QueryIntAttribute("tilewidth", &tileWidth);
		tst->QueryIntAttribute("tileheight", &tileHeight);
		tst->QueryIntAttribute("tilecount", &tileCount);

		std::cout << "gid: " << firstGid << "\n";
		std::cout << "tileCount: " << tileCount << "\n";
		std::cout << "tileWidth: " << tileWidth << "\n";
		std::cout << "tileHeight: " << tileHeight << "\n";

		for (XMLElement * ele = tst->FirstChildElement(); ele != nullptr; ele = ele->NextSiblingElement())
		{
			std::string name = ele->Name();
			if (name == "image")
			{
				std::string imageSource = ele->Attribute("source");

				m_tileTexture = assets.get<sf::Texture>("assets/tilesets/" + imageSource);


				for (int i = 0; i < tileCount; ++i)
				{
					int columns;
					int rows;

					tst->QueryIntAttribute("columns", &columns);
					rows = tileCount / columns;

					int gid = firstGid + i;
					sf::IntRect rect;
					rect.left = i % columns * tileWidth;
					rect.top = i / rows * tileHeight;
					rect.width = tileWidth;
					rect.height = tileHeight;

					Tile tile;
					tile.source = imageSource;
					tile.textureRect = rect;
					m_tiles[gid] = tile;
				}
			}
			else if (name == "tile")
			{
				int localId;
				ele->QueryIntAttribute("id", &localId);
				
				std::string imageSource = ele->FirstChildElement("image")->Attribute("source");
				int width, height;
				ele->FirstChildElement("image")->QueryIntAttribute("width", &width);
				ele->FirstChildElement("image")->QueryIntAttribute("height", &height);

				int gid = firstGid + localId;
				sf::IntRect rect;
				rect.left = 0;
				rect.top = 0;
				rect.width = width;
				rect.height = height;

				Tile tile;
				tile.source = imageSource;
				tile.textureRect = rect;
				m_tiles[gid] = tile;
			}
		}
	}


	//build vertex array

	m_tileVertices.setPrimitiveType(sf::PrimitiveType::Quads);
	
	for (std::size_t y = 0; y < m_data.size(); ++y)
	{
		for (std::size_t x = 0; x < m_data[y].size(); ++x)
		{
			const Tile & t = m_tiles[m_data[y][x]];

			sf::Vertex a, b, c, d;

			a.position = sf::Vector2f(static_cast<float>(x * m_tileSize.x), static_cast<float>(y * m_tileSize.y));
			b.position = sf::Vector2f(static_cast<float>((x + 1) * m_tileSize.x), static_cast<float>(y * m_tileSize.y));
			c.position = sf::Vector2f(static_cast<float>((x + 1) * m_tileSize.x), static_cast<float>((y + 1) * m_tileSize.y));
			d.position = sf::Vector2f(static_cast<float>(x * m_tileSize.x), static_cast<float>((y + 1) * m_tileSize.y));


			a.texCoords = sf::Vector2f(static_cast<float>(t.textureRect.left), static_cast<float>(t.textureRect.top)) + sf::Vector2f(.5f, .5f);
			b.texCoords = sf::Vector2f(static_cast<float>(t.textureRect.left + t.textureRect.width), static_cast<float>(t.textureRect.top)) + sf::Vector2f(-.5f, 0.5f);
			c.texCoords = sf::Vector2f(static_cast<float>(t.textureRect.left + t.textureRect.width), static_cast<float>(t.textureRect.top + t.textureRect.height)) + sf::Vector2f(-.5f, -.5f);
			d.texCoords = sf::Vector2f(static_cast<float>(t.textureRect.left), static_cast<float>(t.textureRect.top + t.textureRect.height)) + sf::Vector2f(.5f, -.5f);

			m_tileVertices.append(a);
			m_tileVertices.append(b);
			m_tileVertices.append(c);
			m_tileVertices.append(d);
		}
	}

	//load objs;

	XMLElement * backgroundLayer = getElementWithAttribute(map, "objectgroup", "name", "Background Layer");
	for (backgroundLayer; backgroundLayer != nullptr; backgroundLayer = backgroundLayer->NextSiblingElement("objectgroup"))
	{
		XMLElement * properties = backgroundLayer->FirstChildElement("properties");
		ObjectLayer layer;
		float parallaxFactor = 1.f;
		if (properties)
		{
			XMLElement * parallaxFactorProperty = getElementWithAttribute(properties, "property", "name", "Parallax Factor");
			if (parallaxFactorProperty)
				parallaxFactorProperty->QueryFloatAttribute("value", &parallaxFactor);
		}
		std::cout << "Layer: " << parallaxFactor << "\n";
		layer.parallaxFactor = parallaxFactor;
		for (XMLElement * object = backgroundLayer->FirstChildElement("object"); object != nullptr; object = object->NextSiblingElement("object"))
		{
			int gid;
			float x, y, w, h;
			object->QueryIntAttribute("gid", &gid);
			object->QueryFloatAttribute("x", &x);
			object->QueryFloatAttribute("y", &y);
			object->QueryFloatAttribute("width", &w);
			object->QueryFloatAttribute("height", &h);
			y -= h;
			
			sf::Texture * texture = assets.get<sf::Texture>("assets/tilesets/" + m_tiles[gid].source);
			sf::Sprite sprite;
			sprite.setTexture(*texture);
			sprite.setPosition(x, y);
			float scaleX = w / texture->getSize().x;
			float scaleY = h / texture->getSize().y;
			sprite.setScale(scaleX, scaleY);

			std::cout << gid << ": " << x << ", " << y << ", " << w << ", " << h << "\n";

			//look for color(optional)
			XMLElement * properties = object->FirstChildElement("properties");
			if (properties)
			{
				XMLElement * colorProperty = getElementWithAttribute(properties, "property", "name", "Color");
				if (colorProperty)
				{
					std::string color = colorProperty->Attribute("value");
					std::cout << "color: " << color << "\n";
					unsigned long  rgb = strtoul(color.substr(1).c_str(), nullptr, 16);
					std::cout << "rgb: " << rgb << "\n";
					//argb
					unsigned a = (rgb >> 24) & 255;
					unsigned r = (rgb >> 16) & 255;
					unsigned g = (rgb >> 8) & 255;
					unsigned b = (rgb >> 0) & 255;
					std::cout << "R: " << r << ", g: " << g << " b: " << b << ", a: " << a << "\n";
					sprite.setColor(sf::Color(r, g, b, a));
				}
			}
			layer.objects.push_back(sprite);

		}
		m_backgroundLayers.push_back(layer);
	}
	return true;
}


void RenderMap::drawBackground(sf::RenderTarget & target) const
{
	sf::View temp = target.getView();

	for (auto & l : m_backgroundLayers)
	{
		sf::Vector2f center = temp.getCenter();
		center *= l.parallaxFactor;
		sf::View parallaxView = temp;
		parallaxView.setCenter(center);
		target.setView(parallaxView);

		for (auto & s : l.objects)
		{
			target.draw(s);
		}
	}

	target.setView(temp);
}

void RenderMap::drawTiles(sf::RenderTarget & target) const
{
	sf::RenderStates states;
	states.texture = m_tileTexture;
	target.draw(m_tileVertices, states);
}
