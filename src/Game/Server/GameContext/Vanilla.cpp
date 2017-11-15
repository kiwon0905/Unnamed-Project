#include "Vanilla.h"
#include "Core/Server/Server.h"
#include "Game/Server/Entity/Human.h"

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
		Human * h = m_gameWorld.createEntity<Human>(p.getPeerId(), sf::Vector2f(100.f, 100.f));
		p.setEntity(h);

		if (i % 2 == 0)
			p.setTeam(Team::A);
		else
			p.setTeam(Team::B);
		++i;
	}

	createCrates();
}

bool Vanilla::checkRound(Team & team)
{

	return false;
}
