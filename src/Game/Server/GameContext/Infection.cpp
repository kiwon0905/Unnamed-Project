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
void Infection::onRoundStart()
{

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
