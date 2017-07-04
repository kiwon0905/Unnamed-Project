#include "Client.h"

#ifdef _DEBUG
	#include <stdlib.h>
	#include <crtdbg.h>
	#define _CRTDBG_MAP_ALLOC
	#ifndef DBG_NEW
		#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
		#define new DBG_NEW
	#endif
#endif


#include "Core/Utility.h"
int main()
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	Client client;
	client.run();
}



/*
#include "Graph.h"
#include "Core/Utility.h"
#include <SFML/Graphics.hpp>
#include "Game/Aabb.h"

#include "AnimatedSprite.h"
int main()
{
	sf::RenderWindow window;

	window.create(sf::VideoMode(1600, 900), "");

	sf::Clock clock;


	sf::Texture texture;
	texture.loadFromFile("gnu_from_gnu.png");


	//0, 56, 112
	//90
	Animation walkRight;
	walkRight.addFrame({ 56, 80, 56, 80 });
	walkRight.addFrame({ 112, 80, 56, 80 });
	walkRight.addFrame({ 56, 80, 56, 80 });
	walkRight.addFrame({ 0, 80, 56, 80 });
	walkRight.setSpriteSheet(texture);
	AnimatedSprite sprite(sf::seconds(.1));
	sprite.setPosition({ 500.f, 500.f });
	sprite.setAnimation(walkRight);
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		sf::Time dt = clock.restart();
		sprite.update(dt);
		sprite.setScale(-1.f, 1.f);
		window.clear();
		window.draw(sprite);
		window.display();
	}

	return 0;
}*/