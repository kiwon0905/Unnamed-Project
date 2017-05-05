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

int randInt(int min, int max)
{
	return rand() % (max + 1 - min) + min;
}
int main()
{

	sf::RenderWindow window;

	window.create(sf::VideoMode(1600, 900), "", sf::Style::Default, sf::ContextSettings(0, 0, 8));

	window.setVerticalSyncEnabled(true);
	sf::CircleShape shape(30.f);
	shape.setFillColor(sf::Color::Green);

	sf::Vector2f pos, prevPos;
	sf::Clock clock;
	sf::Time accum;

	sf::Texture texture;
	texture.loadFromFile("hi.png");
	sf::Sprite spr;
	spr.setTexture(texture);

	while (window.isOpen())
	{
		accum += clock.restart();
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		while (accum >= sf::seconds(1 / 50.f))
		{
			accum -= sf::seconds(1 / 50.f);
			sf::Vector2f vel;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			{
				vel.y = -600.f;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			{
				vel.y = 600.f;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			{
				vel.x = -600.f;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			{
				vel.x = 600.f;
			}
			prevPos = pos;
			pos += vel * sf::seconds(1 / 50.f).asSeconds();
		
		}
		float alpha = accum / sf::seconds(1 / 50.f);

		sf::Vector2f renderPos = lerp(prevPos, pos, alpha);
		shape.setPosition(renderPos);

		window.clear(sf::Color::Cyan);
		window.draw(spr);
		window.draw(shape);
		sf::View view = window.getDefaultView();
		view.setCenter(renderPos);
		window.setView(view);

		window.display();
	}

	return 0;
}*/