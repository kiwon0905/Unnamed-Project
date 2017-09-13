#include "Crate.h"
#include "Game/Snapshot.h"


Crate::Crate(int id, Client & client, PlayingScreen & screen) :
	Entity(id, EntityType::CRATE, client, screen)
{
}

sf::Vector2f Crate::getCameraPosition(const Snapshot * from, const Snapshot * to, float predictedT, float t) const
{
	return sf::Vector2f();
}

void Crate::render(const Snapshot * from, const Snapshot * to, float predictedT, float t)
{
	const NetCrate * c0 = static_cast<const NetCrate*>(from->getEntity(m_id));


	sf::Vector2f pos = static_cast<sf::Vector2f>(c0->pos) / 100.f;
	sf::RectangleShape r;
	r.setSize({ 70.f, 70.f });
	r.setPosition(pos);
	r.setFillColor(sf::Color(165, 42, 42));
	m_client->getWindow().draw(r);

}
