#include "Item.h"

Item::Item(int id, Client & client, PlayingScreen & screen):
	Entity(id, EntityType::ITEM, client, screen)
{
}

const void * Item::find(const Snapshot & s)
{
	return nullptr;
}

sf::Vector2f Item::getCameraPosition() const
{
	return sf::Vector2f();
}

void Item::render()
{
}
