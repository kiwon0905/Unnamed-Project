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
#include "Game/Aabb.h"

int randInt(int min, int max)
{
	return rand() % (max + 1 - min) + min;
}


int main()
{
	std::cout << std::fixed << std::setprecision(6);
	sf::RenderWindow window;

	window.create(sf::VideoMode(1600, 900), "", sf::Style::Default, sf::ContextSettings(0, 0, 8));



	sf::Vector2f pos, prevPos;
	sf::Clock clock;
	sf::Time accum;

	Aabb w{ -300.f, -300.f, 500.f, 200.f };

	Aabb aabb;
	aabb.w = 100.f;
	aabb.h = 100.f;
	//sf::Vector2f vel = { 600.f, 600.f };
	sf::Vector2f vel;
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
			vel = { 0.f, 0.f };
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				vel.y = -300.f;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				vel.y = 300.f;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			{
				vel.x = -300.f;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			{
				vel.x = 300.f;
			}
			sf::Vector2f displacement = vel * sf::seconds(1 / 50.f).asSeconds();

			aabb.x = pos.x;
			aabb.y = pos.y;

			prevPos = pos;
		
			
			float time;
			sf::Vector2i norm;
			if (aabb.sweep(displacement, w, time, norm))
			{
				pos.x += displacement.x * time;
				pos.y += displacement.y * time;


				float dot = (displacement.x* norm.y + displacement.y *norm.x)*(1 - time);
				displacement.x = dot * norm.y;
				displacement.y = dot * norm.x;
				std::cout << "!";
			}
			pos += displacement;
			//std::cout << "velocity: " << vel << "\n";
		}
		float alpha = accum / sf::seconds(1 / 50.f);

		sf::Vector2f renderPos = lerp(prevPos, pos, alpha);
		sf::View view = window.getDefaultView();
		view.setCenter(renderPos);
		window.setView(view);
		window.clear(sf::Color::Cyan);
		sf::RectangleShape r;
		r.setOutlineThickness(-1.f);
		r.setOutlineColor(sf::Color::Black);
		r.setPosition(renderPos);
		r.setSize({ 100.f, 100.f });
		window.draw(r);


		sf::RectangleShape r2;
		r2.setSize({ w.w, w.h });
		r2.setPosition({ w.x, w.y });
		r2.setFillColor(sf::Color::Black);
		r2.setOutlineThickness(-1.f);
		r2.setOutlineColor(sf::Color::Yellow);
		window.draw(r2);

		window.display();


		sf::sleep(sf::milliseconds(1));
	}

	return 0;
}*/