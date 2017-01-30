#include "Human.h"
#include "Game/Client/GameWorld.h"
#include "Core/MathUtility.h"

Human::Human(unsigned id):
	Entity(id, EntityType::HUMAN)
{

}

void Human::update(float dt, GameWorld & world)
{

}

void Human::render(Renderer & renderer)
{
	renderer.renderHuman(m_position.x, m_position.y);
}
