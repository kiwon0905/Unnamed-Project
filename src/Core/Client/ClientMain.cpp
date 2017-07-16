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
#include "Core/Client/Particles.h"
int main()
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	Client client;
	client.run();


	/*
	sf::Texture texture;
	if (!texture.loadFromFile("particle.png"))
		std::cout << "failed to load!";

	sf::RenderWindow window(sf::VideoMode(1600, 900), "hi");
	
	sf::Clock clock;
	SmokeParticles particles(texture);
	ParticleEmitter * emitter = new ParticleEmitter;
	particles.addEmitter(emitter);
	emitter->setEmissionRate(2000.f);
	emitter->setParticleLifeTime(2.f);
	emitter->setParticleVelocity(Math::circle(sf::Vector2f(), 25.f));
	emitter->setParticleScale(Math::uniform(.005f, .01f));
	emitter->setParticlePosition(sf::Vector2f());
	emitter->setParticleScale(Math::uniform(.05f, .1f));
	emitter->setParticleColor(sf::Color(231, 231, 231, 10));
	while (window.isOpen())
	{
		sf::Event event;
		float dt = clock.restart().asSeconds();
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A)
				particles.removeEmitter(emitter);
		}
		emitter->setParticlePosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)));
		particles.update(dt);


		window.clear(sf::Color(135, 206, 235));
		sf::RenderStates states;
		states.blendMode = sf::BlendAdd;
		window.draw(particles, states);
		window.display();
	}
	*/
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