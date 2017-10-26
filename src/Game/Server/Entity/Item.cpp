#include "Item.h"

Item::Item(int id, GameContext * context):
	Entity(id, NetObject::ITEM, context)
{
}

void Item::tick(float dt)
{
}

void Item::snap(Snapshot & snapshot) const
{
}
