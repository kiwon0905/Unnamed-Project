#include "Normal.h"

void Normal::startRound()
{
	m_state = LOADING;

	m_map.loadFromFile("map/grass2.xml");


	for (std::size_t i = 0; i < m_peers.size(); ++i)
	{
		auto & p = m_peers[i];

		Human * h = m_gameWorld.createEntity<Human>(p->getId(), sf::Vector2f(100.f, 100.f));
		std::cout << "created human for peer " << p->getId() << "\n";
		p->setEntity(h);

		if (i % 2 == 0)
			p->setTeam(Team::A);
		else
			p->setTeam(Team::B);
	}

	for (auto & p : m_peers)
	{
		Packer packer;
		packer.pack(Msg::SV_LOAD_GAME);
		p->send(packer, true);
	}
}

void Normal::checkRound()
{
	int a = 0, b = 0;

	for (const auto & e : m_gameWorld.getEntities(EntityType::HUMAN))
	{
		Human * h = static_cast<Human*>(e.get());
		Peer * p = getPeer(h->getPeerId());
		if (p)
		{
			if (p->getTeam() == Team::A)
				++a;
			else if (p->getTeam() == Team::B)
				++b;
		}
	}
	if (m_tick > 15000 && a == b)
	{
		endRound(Team::NONE);
	}
	/*
	else
	{
		if (a == b && a == 0)
			endRound(Team::NONE);
		else if (a == 0 && b > a)
			endRound(Team::B);
		else if (b == 0 && a > b)
			endRound(Team::A);

	}*/
}
