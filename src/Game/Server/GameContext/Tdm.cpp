#include "Tdm.h"

#include "Game/Server/Entity/Human.h"
#include "Core/Server/Server.h"

Tdm::Tdm(Server * server) :
	GameContext(server)
{

}

std::string Tdm::getName()
{
	return "TDM";
}

void Tdm::loadRound()
{
	m_map.loadFromTmx("map/tdm1.tmx");
}

void Tdm::onRoundStart()
{
	int i = 0;
	for (auto & p : m_players)
	{
		Character * e;
		e = static_cast<Character*>(m_gameWorld.createEntity<Human>(p.getPeerId(), sf::Vector2f(100.f, 100.f)));

		p.setCharacter(e);

		if (i % 2 == 0)
			p.setTeam(Team::A);
		else
			p.setTeam(Team::B);
		++i;
	}
}

void Tdm::tick(float dt)
{
}

void Tdm::snap(Snapshot & snapshot)
{
}

bool Tdm::checkRound(Team & team)
{
	return false;
}

void Tdm::reset()
{
}
