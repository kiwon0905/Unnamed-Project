#include "Cart.h"
#include "Game/Snapshot.h"
#include "Game/NetObject/NetCart.h"
#include "Core/Utility.h"


Cart::Cart(int id, Client & client, PlayingScreen & screen):
	Entity(id, EntityType::CART, client, screen)
{
}

const void * Cart::find(const Snapshot & s)
{
	return s.getEntity(NetObject::CART, m_id);
}

sf::Vector2f Cart::getCameraPosition(const Snapshot * from, const Snapshot * to, float predictedT, float t) const
{
	return sf::Vector2f();
}

void Cart::render(const Snapshot * from, const Snapshot * to, float predictedT, float t)
{
	sf::RenderWindow & target = m_client->getWindow();
	const NetCart * p0 = static_cast<const NetCart*>(from->getEntity(NetObject::CART, m_id));
	const NetCart * p1 = nullptr;

	if (to)
		p1 = static_cast<const NetCart*>(to->getEntity(NetObject::CART, m_id));


	sf::Vector2f pos = static_cast<sf::Vector2f>(p0->pos) / 100.f;
	if (p1)
	{
		pos = Math::lerp(static_cast<sf::Vector2f>(p0->pos) / 100.f, static_cast<sf::Vector2f>(p1->pos) / 100.f, t);
	}

	sf::RectangleShape r;
	r.setSize({ 280.f, 140.f });
	r.setFillColor(sf::Color::Yellow);
	r.setPosition(pos);
	target.draw(r);

}
