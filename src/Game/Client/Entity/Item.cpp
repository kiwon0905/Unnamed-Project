#include "Item.h"

Item::Item(int id, Client & client, PlayingScreen & screen):
	Entity(id, NetObject::ITEM, client, screen)
{
}

const void * Item::find(const Snapshot & s)
{
	return nullptr;
}

sf::Vector2f Item::getCameraPosition(const Snapshot * from, const Snapshot * to, float predictedT, float t) const
{
	return sf::Vector2f();
}

void Item::render(const Snapshot * from, const Snapshot * to, float predictedT, float t)
{
}
