#pragma once
#include "Core/Client/Client.h"
#include "Core/Packet.h"

class GameWorld
{
public:
	void update(float dt, Client & client);
	void render(Client & client);
	void handlePacket(Packet & packet);
private:
};