#include "Vanilla.h"
#include "Game/Server/Entity/Human.h"
#include "Game/Server/Entity/Zombie.h"
#include "Game/Server/Entity/Cart.h"

#include "Core/Server/Server.h"

Vanilla::Vanilla(Server * server):
	GameContext(server)
{
}

std::string Vanilla::getName()
{
	return "Vanilla";
}

void Vanilla::onRoundStart()
{
	std::string cartStartPos, cartEndPos;
	m_map.getProperty("cart_start_position", cartStartPos);
	m_map.getProperty("cart_end_position", cartEndPos);

	sscanf_s(&cartStartPos[0], "(%f, %f)", &m_cartStartPos.x, &m_cartStartPos.y);
	sscanf_s(&cartEndPos[0], "(%f, %f)", &m_cartEndPos.x, &m_cartEndPos.y);






	int i = 0;
	for (auto & p : m_players)
	{
		Character * e;
		if(i % 2)
			e = static_cast<Character*>(m_gameWorld.createEntity<Human>(p.getPeerId(), sf::Vector2f(100.f, 100.f)));
		else
			e = static_cast<Character*>(m_gameWorld.createEntity<Zombie>(p.getPeerId(), sf::Vector2f(100.f, 100.f)));

		p.setCharacter(e);

		if (i % 2 == 0)
			p.setTeam(Team::A);
		else
			p.setTeam(Team::B);
		++i;
	}

	m_gameWorld.createEntity<Cart>(sf::Vector2f{ m_cartStartPos.x, m_cartStartPos.y }, Team::A);

}

bool Vanilla::checkRound(Team & team)
{

	return false;
}
