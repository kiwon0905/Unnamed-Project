#include "GameWorld.h"
#include "Core/Protocol.h"
#include "Game/Snapshot.h"


void GameWorld::update(float dt, Client & client)
{
	Packer packer;
	packer.pack(Msg::CL_INPUT);
	packer.pack(client.getInput()->getBits());
	client.getNetwork()->send(packer, false);
	//packer.pack
	//Packet packet;
	//packet << Msg::CL_INPUT << client.getInput()->getBits();
	//client.getNetwork()->send(packet, false);
}

void GameWorld::render(Client & client)
{
}

void GameWorld::handlePacket(Unpacker & unpacker)
{
	//Msg msg;
	//packet >> msg;
	//if (msg == Msg::SV_SNAPSHOT)
	//{
	//	Snapshot ss;
	//	packet >> ss;
	//	std::cout << "entity count: " << ss.entityCount << "";
	//}
}
