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
#include <SFML/Graphics.hpp>

int randInt(int min, int max)
{
	return rand() % (max + 1 - min) + min;
}
int main()
{
	sf::Font font;
	font.loadFromFile("arial.ttf");
	srand(time(NULL));
	sf::RenderWindow window;
	window.create(sf::VideoMode(1600, 900), "", sf::Style::Default, sf::ContextSettings(0, 0, 8));
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

	Graph g(-50.f, 50.f, font, "data");
	g.setSize({ 150.f, 150.f });
	g.setMaxSampleSize(128);
	for (int i = 0; i < 220; ++i)
	{
		float f = randInt(-150, 50);
		g.addSample(f);
		std::cout << "f: " << f << "\n";
	}

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.draw(g);
		window.display();
	}

	return 0;
}

*/