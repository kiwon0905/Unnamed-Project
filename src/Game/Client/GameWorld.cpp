#include "GameWorld.h"
#include "Game/Snapshot.h"


void GameWorld::update(float dt, Client & client)
{
	Packet packet;
	packet << Msg::CL_INPUT << client.getInput()->getBits();
	client.getNetwork()->send(packet, false);
}

void GameWorld::render(Client & client)
{
}

void GameWorld::handlePacket(Packet & packet)
{
	Msg msg;
	packet >> msg;
	if (msg == Msg::SV_SNAPSHOT)
	{
		Snapshot ss;
		packet >> ss;
		std::cout << "entity count: " << ss.entityCount << "";
	}
}
