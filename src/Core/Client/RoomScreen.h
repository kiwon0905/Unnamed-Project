#pragma once

#include "Screen.h"

class RoomScreen : public Screen
{
public:
	virtual void onEnter(Client & client);
	virtual void handleNetEvent(ENetEvent & netEv, Client & client);
	virtual void handlePacket(Unpacker & unpacker, const ENetAddress & addr, Client & client);
	virtual void update(Client & client);
	virtual void render(Client & client);
	virtual void onExit(Client & client);
	virtual void onObscure(Client & client);
	virtual void onReveal(Client & client);
private:
};