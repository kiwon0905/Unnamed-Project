#include "Cart.h"
#include "Game/Snapshot.h"
#include "Game/Server/GameContext.h"
#include "Game/NetObject/NetCart.h"
#include "Game/Server/Entity/Character.h"

Cart::Cart(int id, GameContext * context, const sf::Vector2f & pos, Team team):
	Entity(id, EntityType::CART, context, pos),
	m_team(team)
{
	m_size = { 280.f, 140.f };
}

void Cart::tick(float dt)
{
	int myTeam = 0;
	int enemyTeam = 0;


	Aabb aabb{ m_position, m_size };

	for (auto e : m_context->getWorld().getEntities(EntityType::CHARACTER))
	{
		Character * c = static_cast<Character*>(e);
		Player * p = m_context->getPlayer(c->getPeerId());

		Aabb caabb{ c->getPosition(), c->getSize() };

		if (aabb.intersects(caabb))
		{		
			if (p->getTeam() == m_team)
			{
				myTeam++;
			}
			else
			{
				enemyTeam++;
			}
		}
	}

	if (myTeam > 0 && enemyTeam == 0)
	{
		if (myTeam == 1)
			m_velocity.x = 50.f;
		else if (myTeam == 2)
			m_velocity.x = 75.f;
		else
			m_velocity.x = 100.f;
	}
	else
	{
		m_velocity.x = 0.f;
	}
	
	sf::Vector2f d = m_velocity * dt;

	sf::Vector2f out;
	sf::Vector2i norm;
	sf::Vector2i tile;
	if (m_context->getMap().sweepCharacter(aabb, d, out, norm, tile))
	{
		if (norm.x)
			m_velocity.x = 0.f;
		if (norm.y)
			m_velocity.y = 0.f;
		d = out;
	}


	m_position += d;

}

void Cart::snap(Snapshot & snapshot) const
{
	NetCart * nc = static_cast<NetCart *>(snapshot.addEntity(NetObject::CART, m_id));
	if (nc)
	{
		nc->pos.x = static_cast<int>(std::round(m_position.x * 100.f));
		nc->pos.y = static_cast<int>(std::round(m_position.y * 100.f));
	}

	

}
