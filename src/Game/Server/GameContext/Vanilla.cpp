#include "Vanilla.h"
#include "Game/Server/Entity/Human.h"
#include "Game/Server/Entity/Zombie.h"
#include "Core/Server/Server.h"

#include "Game/NetObject/NetGameDataControl.h"
#include "Game/Snapshot.h"
Vanilla::Vanilla(Server * server):
	GameContext(server)
{
}

std::string Vanilla::getName()
{
	return "Vanilla";
}

void Vanilla::loadRound()
{
	m_map.loadFromTmx("map/control1.tmx");
}

void Vanilla::onRoundStart()
{
	std::string controlPoint;
	m_map.getProperty("control point", controlPoint);
	sscanf_s(&controlPoint[0], "( %f, %f, %f, %f)", &m_controlPoint.x, &m_controlPoint.y, &m_controlPoint.w, &m_controlPoint.h);

	const Map::GameObject * o = m_map.getGameObject("Control Point");
	m_controlPoint.x = o->rect.left;
	m_controlPoint.y = o->rect.top;
	m_controlPoint.w = o->rect.width;
	m_controlPoint.h = o->rect.height;

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

void Vanilla::tick(float dt)
{
	int teamA = 0;
	int teamB = 0;

	for (auto c : m_gameWorld.getEntities(EntityType::CHARACTER))
	{
		Aabb caabb{ c->getPosition(), c->getSize() };
		if (m_controlPoint.intersects(caabb))
		{
			Player * p = getPlayer(static_cast<Character*>(c)->getPeerId());
			if (p->getTeam() == Team::A)
				++teamA;
			else if (p->getTeam() == Team::B)
				++teamB;
		}
	}
	

	m_capturingTeam = Team::NONE;
	if (teamA > 0 && teamB == 0 && m_controllingTeam != Team::A)
		m_capturingTeam = Team::A;
	else if(teamB > 0 && teamA == 0 && m_controllingTeam != Team::B)
		m_capturingTeam = Team::B;


	//capture the point if controlling team != capturing team
	if (m_capturingTeam != Team::NONE)
	{
		if (m_capturingTeam != m_controllingTeam)
		{
			if (m_capturingTeam == Team::A)
			{
				m_captureProgressDecay = 100;

				//need to undo other team's progres first;
				if (m_captureProgressB > 0)
					--m_captureProgressB;
				else
				{
					++m_captureProgressA;
					if (m_captureProgressA == 100)
					{
						m_controllingTeam = Team::A;
						m_captureProgressA = 0;
					}
				}

			}
			else if (m_capturingTeam == Team::B)
			{
				m_captureProgressDecay = 100;


				if (m_captureProgressA > 0)
					--m_captureProgressA;
				else
				{
					++m_captureProgressB;
					if (m_captureProgressB == 100)
					{
						m_controllingTeam = Team::B;
						m_captureProgressB = 0;
					}
				}
			}
		}
	}

	//capture progress decays after certain period of time
	if(teamA == 0 && teamB == 0)
	{
		if (m_captureProgressDecay > 0)
			m_captureProgressDecay--;

		if (m_captureProgressDecay == 0)
		{
			if (m_captureProgressA > 0)
				m_captureProgressA--;

			else if (m_captureProgressB > 0)
				m_captureProgressB--;
		}
		
	}
	//control progress
	if (m_controllingTeam == Team::A)
	{
		if (m_controlProgressA < 499)
			++m_controlProgressA;
		else if (m_controlProgressA == 499 && teamB == 0)
			++m_controlProgressA;
	}
	else if (m_controllingTeam == Team::B)
	{
		if (m_controlProgressB < 499)
			++m_controlProgressB;
		else if (m_controlProgressB == 499 && teamA == 0)
			++m_controlProgressB;
	}


}

void Vanilla::snap(Snapshot & snapshot)
{
	NetGameDataControl * ngdc = static_cast<NetGameDataControl *>(snapshot.addEntity(NetObject::GAME_DATA_CONTROL, 0));
	if (ngdc)
	{
		ngdc->controllingTeam = m_controllingTeam;
		ngdc->controlProgressA = m_controlProgressA;
		ngdc->controlProgressB = m_controlProgressB;

		ngdc->captureProgressA = m_captureProgressA;
		ngdc->captureProgressB = m_captureProgressB;
		ngdc->capturingTeam = m_capturingTeam;
	}
}

bool Vanilla::checkRound(Team & team)
{
	if (m_controlProgressA == 500)
	{
		team = Team::A;
		return true;
	}
	if (m_controlProgressB == 500)
	{
		team = Team::B;
		return true;
	}
	return false;
}

void Vanilla::reset()
{
	GameContext::reset();
	m_controllingTeam = Team::NONE;
	m_controlProgressA = 0;
	m_controlProgressB = 0;
	m_capturingTeam = Team::NONE;
	m_captureProgressA = 0;
	m_captureProgressB = 0;
}
