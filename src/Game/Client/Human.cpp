#include "Human.h"
#include "Game/Client/GameWorld.h"
#include "Core/MathUtility.h"

Human::Human(int id):
	Entity(id, EntityType::HUMAN)
{

}

void Human::update(float dt, GameWorld & world)
{
}

void Human::renderPast(Renderer & renderer, const NetEntity * from, const NetEntity * to)
{
	std::cout << "render past: " << m_id << "\n";
}

void Human::renderFuture(Renderer & renderer, const CharacterCore & core)
{
	renderer.renderHuman(core.getPosition().x, core.getPosition().y);
}

