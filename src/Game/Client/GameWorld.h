#pragma once
#include "Core/Client/Client.h"
#include "Core/Packer.h"

class GameWorld
{
public:
	void update(float dt, Client & client);
	void render(Client & client);
	void handlePacket(Unpacker & unpacker);
private:
};