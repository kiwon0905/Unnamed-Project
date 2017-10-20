#include "Vanilla.h"
#include "Core/Server/Server.h"
#include "Game/Server/Entity/Human.h"

Normal::Normal(Server * server):
	GameContext(server)
{

}

std::string Normal::getName()
{
	return "Vanilla";
}

void Normal::prepareRound()
{
	m_map.loadFromFile("map/grass2.xml");

	int i = 0;
	for (auto & p : m_server->getPeers())
	{
		Human * h = m_gameWorld.createEntity<Human>(p->getId(), sf::Vector2f(100.f, 100.f));
		std::cout << "created human for peer " << p->getId() << "\n";
		p->setEntity(h);

		if (i % 2 == 0)
			p->setTeam(Team::A);
		else
			p->setTeam(Team::B);
		++i;
	}

	createCrates();
}

bool Normal::checkRound(Team & team)
{
	if (m_tick > 150)
	{
		team = Team::NONE;
		return true;
	}
	return false;
}
