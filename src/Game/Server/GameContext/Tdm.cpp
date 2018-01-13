#include "Tdm.h"
#include "Game/NetObject/NetGameDataTdm.h"
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
	NetGameDataTdm  * ngdt = static_cast<NetGameDataTdm*>(snapshot.addEntity(NetObject::GAME_DATA_TDM, 0));
	if (ngdt)
	{
		ngdt->scoreA = m_scoreA;
		ngdt->scoreB = m_scoreB;
	}
}

bool Tdm::checkRound(Team & team)
{
	return false;
}

void Tdm::reset()
{
}

void Tdm::onCharacterDeath(int killedPeer, int killerPeer, const std::unordered_map<int, int>& assisters)
{
	GameContext::onCharacterDeath(killedPeer, killerPeer, assisters);
	Player * killed = getPlayer(killedPeer);
	Player * killer = getPlayer(killerPeer);
	if (killed && killer)
	{
		if (killed->getTeam() == Team::A)
		{
			++m_scoreB;
		}
		else if (killed->getTeam() == Team::B)
		{
			++m_scoreA;
		}
	}

	if (killed)
		killed->setRespawnTick(250);
}
