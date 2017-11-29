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
	sf::FloatRect r;
	//load background images
	XMLElement * backgroundLayer = getElementWithAttribute(map, "objectgroup", "name", "Background Layer");
	if (backgroundLayer)
	{
		for (XMLElement * object = backgroundLayer->FirstChildElement("object"); object != nullptr; object = object->NextSiblingElement("object"))
		{
			Object obj;
			object->QueryIntAttribute("gid", &obj.gid);
			object->QueryFloatAttribute("x", &obj.x);
			object->QueryFloatAttribute("y", &obj.y);
			object->QueryFloatAttribute("width", &obj.w);
			object->QueryFloatAttribute("height", &obj.h);

			obj.y -= obj.h;

			//get properties
			XMLElement * properties = object->FirstChildElement("properties");
			if (properties)
			{
				for (XMLElement * property = properties->FirstChildElement("property"); property != nullptr; property = property->NextSiblingElement("property"))
				{
					if (property->Attribute("name", "Repeat Texture"))
					{
						if (property->Attribute("value", "true"))
						{
							obj.repeatTexture = true;
							std::cout << "repeat req\n";
						}
					}
				}
			}

			m_backgroundObjects.push_back(obj);
		}
	}

	for (auto & o : m_backgroundObjects)
	{
		sf::Texture * texture = assets.get<sf::Texture>("assets/tilesets/" + m_tiles[o.gid].source);
		if (o.repeatTexture)
		{
			texture->setRepeated(true);

			//seems this adding/subtracting 1 helps to get rid of the artifacts
			sf::IntRect r;
			r.left = 1;
			r.top = 1;
			r.height = texture->getSize().y - 1;
			r.width = m_tileSize.x * m_size.x - 1;
			o.sprite.setTextureRect(r);
		}
		o.sprite.setTexture(*texture);
		o.sprite.setPosition(o.x, o.y);
		o.sprite.setColor(sf::Color::White);
		float scaleFactor = static_cast<float>(o.h) / texture->getSize().y;
		std::cout << "texture size: " << texture->getSize().x << ", " << texture->getSize().y << "\n";
		std::cout << "Scale factor: " << scaleFactor << "\n";
		o.sprite.setScale(scaleFactor, scaleFactor);
		std::cout << "x: " << o.x << ", y: " << o.y << ", w: " << o.w << ", h: " << o.h << "\n";
	}
	return true;
}


void RenderMap::drawBackground(sf::RenderTarget & target) const
{
	sf::View view = target.getView();
	sf::Vector2f center = target.getView().getCenter();
	center *= .7f;
	sf::View parallaxView = view;
	parallaxView.setCenter(center);
	target.setView(parallaxView);
	for (auto & o : m_backgroundObjects)
	{
		target.draw(o.sprite);
	}
	target.setView(view);
}

void RenderMap::drawTiles(sf::RenderTarget & target) const
{
	sf::RenderStates states;
	states.texture = m_tileTexture;
	target.draw(m_tileVertices, states);
}
