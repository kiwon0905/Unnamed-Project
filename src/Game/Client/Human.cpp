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

void Human::renderPast(Renderer & renderer, const NetEntity * from, const NetEntity * to, float t)
{
	const NetHuman * h0 = static_cast<const NetHuman*>(from);
	const NetHuman * h1 = static_cast<const NetHuman*>(to);
	sf::Vector2f pos = h0->position;

	if(to)
		pos = lerp(h0->position, h1->position, t);

	renderer.renderHuman(pos.x, pos.y);
}

void Human::renderFuture(Renderer & renderer, const CharacterCore & core)
{
	renderer.renderHuman(core.getPosition().x, core.getPosition().y);
}

