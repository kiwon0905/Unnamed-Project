#pragma once
#include "Game/Client/Entity.h"

class Item : public Entity
{
public:
	Item(int id, Client & client, PlayingScreen & screen);

	const void * find(const Snapshot & s);

	sf::Vector2f getCameraPosition() const;
	void render();

private:


};