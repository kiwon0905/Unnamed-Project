#include "Human.h"
#include "Game/Client/GameWorld.h"

Human::Human(unsigned id):
	Entity(id, EntityType::HUMAN)
{

}

void Human::update(float dt, GameWorld & world)
{
	auto & snapshots = world.getSnapshots();
	if (!snapshots.empty())
	{
		if (snapshots.back().entityInfo.count(m_id))
		{
			const GameWorld::Snapshot::EntityInfo & info = snapshots.back().entityInfo.at(m_id);
			m_position.x = info.x;
			m_position.y = info.y;
		}
	}
}

void Human::render(Renderer & renderer)
{
	renderer.renderHuman(m_position.x, m_position.y);
}
