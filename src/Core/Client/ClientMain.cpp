#include "Client.h"
#include "Core/Packer.h"

#ifdef _DEBUG
	#include <stdlib.h>
	#include <crtdbg.h>
	#define _CRTDBG_MAP_ALLOC
	#ifndef DBG_NEW
		#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
		#define new DBG_NEW
	#endif
#endif

#include "Core/Packer.h"

enum A
{
	First,a, b, c, d, LAST
};



int main()
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Client client;
	client.run();

}


/*

#include <SFML/Graphics.hpp>

int main()
{
	sf::RenderWindow window(sf::VideoMode(1000, 800), "SFML works!");
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);
	
	sf::Clock clock;
	sf::Time elapsed = sf::Time::Zero;
	sf::Vector2f pos;
	while (window.isOpen())
	{
		elapsed += clock.restart();
		while (elapsed >= sf::seconds(1 / 60.f))
		{
			elapsed -= sf::seconds(1 / 60.f);
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
				pos.x -= 5.f;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
				pos.x += 5.f;
		}

		window.clear();
		shape.setPosition(pos);
		window.draw(shape);
		window.display();
	}

	return 0;
}

*/