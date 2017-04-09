#include "Human.h"
#include "Core/Utility.h"

#include <SFML/Graphics.hpp>

Human::Human(int id):
	Entity(id, EntityType::HUMAN)
{

}

void Human::update(float dt, GameWorld & world)
{
}

void Human::renderPast(const NetItem * from, const NetItem * to, float t, sf::RenderTarget & target)
{
	const NetHuman * h0 = static_cast<const NetHuman*>(from);
	const NetHuman * h1 = static_cast<const NetHuman*>(to);
	sf::Vector2f pos = h0->position;

	if(to)
		pos = lerp(h0->position, h1->position, t);

	sf::RectangleShape r;
	r.setSize({ 50.f, 50.f });
	r.setPosition(pos);
	target.draw(r);
}

void Human::renderFuture(const CharacterCore & prevCore, const CharacterCore & currentCore, float t)
{
	sf::Vector2f pos = lerp(prevCore.getPosition(), currentCore.getPosition(), t);
	
}

