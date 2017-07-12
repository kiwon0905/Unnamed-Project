#include "Infection.h"

void Infection::startRound()
{
	m_state = LOADING;

	m_map.loadFromFile("map/grass.xml");


	for (std::size_t i = 0; i < m_peers.size(); ++i)
	{
		auto & p = m_peers[i];

		Human * h = m_gameWorld.createEntity<Human>(p->getId(), sf::Vector2f(100.f, 100.f));
		p->setEntity(h);

		//Zombie * z = m_gameWorld.createEntity<Zombie>(p.get());
		//p->setEntity(z);
	}

	for (auto & p : m_peers)
	{
		Packer packer;
		packer.pack(Msg::SV_LOAD_GAME);
		p->send(packer, true);
	}
}

void Infection::checkRound()
{
	if (m_tick > 15000)
	{
		endRound(Team::A);
	}
}
