#include "Vanilla.h"
#include "Core/Server/Server.h"
#include "Game/Server/Entity/Human.h"
#include "Game/Server/Entity/Zombie.h"


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
}

bool Vanilla::checkRound(Team & team)
{

	return false;
}
