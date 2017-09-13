#include "Item.h"

Item::Item(int id, Client & client, PlayingScreen & screen):
	Entity(id, EntityType::ITEM, client, screen)
{
}

sf::Vector2f Item::getCameraPosition(const Snapshot * from, const Snapshot * to, float predictedT, float t) const
{
	return sf::Vector2f();
}

void Item::render(const Snapshot * from, const Snapshot * to, float predictedT, float t)
{
}
