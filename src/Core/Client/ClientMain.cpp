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
#include "Game/Client/RenderMap.h"
#include "Core/Client/AssetManager.h"
int main()
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif


	Client client;
	client.run();
	
	/*AssetManager mgr;
	RenderMap map;
	map.loadFromTmx("map/control1.tmx");
	map.loadTextures(mgr);*/
}


/*
#include <TGUI/TGUI.hpp>

int main()
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	sf::RenderWindow window{ { 800, 600 }, "Window" };
	tgui::Gui gui{ window }; // Create the gui and attach it to the window

	tgui::Texture texture;
	texture.load("assets/prev.png", {}, {}, true);
	auto prev = tgui::Picture::create(texture);
	auto onClick = []()
	{
		std::cout << "clicked\n";
	};
	prev->onClick.connect(onClick);
	gui.add(prev);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			gui.handleEvent(event); // Pass the event to the widgets
		}

		window.clear();
		gui.draw(); // Draw all widgets
		window.display();
	}
}*/