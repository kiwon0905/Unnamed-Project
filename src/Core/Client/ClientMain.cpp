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


#include "Game/Map.h"

int main()
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	Client client;
	client.run();
}

/*
#include "Core/MathUtility.h"
#include <SFML/Graphics.hpp>
int main()
{
	sf::RenderWindow window(sf::VideoMode(1000, 800), "SFML works!");
	sf::RenderTexture renderTexture;
	renderTexture.create(1000, 800);
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);
	
	sf::Clock clock;
	sf::Time elapsed = sf::Time::Zero;
	sf::Vector2f pos;
	sf::Vector2f oldPos;


	while (window.isOpen())
	{
		sf::Time dt = clock.restart();
		elapsed += dt;
		std::cout << "dt: " << dt.asSeconds() << "\n";
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		
		while (elapsed >= sf::seconds(1 / 60.f))
		{
			std::cout << "tick\n";
			elapsed -= sf::seconds(1 / 60.f);
			oldPos = pos;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
				pos.x -= 5.f;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
				pos.x += 5.f;
		}
		float t = elapsed.asSeconds() / sf::seconds(1 / 60.f).asSeconds();
		
		renderTexture.clear();
		sf::Vector2f renderPos = lerp(oldPos, pos, t);
		sf::View view = renderTexture.getDefaultView();
		view.setCenter(renderPos);
		renderTexture.setView(view);
		renderTexture.draw(shape);
		shape.setPosition(pos);
		renderTexture.display();
		
		window.clear();
		sf::Sprite sprite;
		sprite.setTexture(renderTexture.getTexture());
		sprite.setScale(.5f, .5f);
		window.draw(sprite);

		window.display();
	}



	return 0;
}

*/