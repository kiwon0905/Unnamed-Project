#include "Infection.h"

#include "Game/Server/Entity/Human.h"
#include "Core/Server/Server.h"

Infection::Infection(Server * server) :
	GameContext(server)
{

}

std::string Infection::getName()
{
	return "Infection";
}
void Infection::prepareRound()
{
	m_map.loadFromFile("map/grass.xml");


	for (auto & p : m_server->getPeers())
	{
		Human * h = m_gameWorld.createEntity<Human>(p->getId(), sf::Vector2f(100.f, 100.f));
		p->setEntity(h);

		//Zombie * z = m_gameWorld.createEntity<Zombie>(p.get());
		//p->setEntity(z);
	}

}

bool Infection::checkRound(Team & team)
{
	if (m_tick > 150)
	{
		team = Team::NONE;
		return true;
	}
	return false;
}
