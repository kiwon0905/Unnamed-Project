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



#include "Game/Snapshot.h"
#include "Core/Rle.h"
#include "Core/Utility.h"

#include "Game/Aabb.h"

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
	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");


	sf::Vector2f size = { 400.f, 100.f };
	sf::Vector2f pos = { 100.f, 100.f };
	sf::Vector2f point = { 600.f, 600.f };

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A)
				point = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
		}

		sf::Vector2i mousePos = sf::Mouse::getPosition(window);

		sf::VertexArray arr{ sf::PrimitiveType::Lines, 2 };
		sf::Vertex v0;
		v0.position = point;
		v0.color = sf::Color::Yellow;
		sf::Vertex v1;
		v1.position = static_cast<sf::Vector2f>(mousePos);
		v1.color = sf::Color::Yellow;
		arr.append(v0);
		arr.append(v1);

		window.clear();

		Aabb aabb{ pos, size };
		float t;
		if (aabb.testLine(v0.position, v1.position, t))
		{
			std::cout << "!";

			sf::Vector2f intersectionPoint = v0.position + (v1.position - v0.position) * t;
			sf::CircleShape c;
			c.setRadius(10.f);
			c.setOrigin(10.f, 10.f);
			c.setPosition(intersectionPoint);
			window.draw(c);

		}


		sf::RectangleShape r;
		r.setSize(size);
		r.setPosition(pos);
		window.draw(r);
		window.draw(arr);
		window.display();
	}

	return 0;
}*/