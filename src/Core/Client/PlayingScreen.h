#pragma once

#include "Screen.h"
#include "Game/Client/GameWorld.h"

class PlayingScreen : public Screen
{
public:
	virtual void onEnter(Client & client);
	virtual void handleEvent(const sf::Event & ev, Client & client);
	virtual void handleNetEvent(ENetEvent & netEv, Client & client);
	virtual void handlePacket(Unpacker & unpacker, const ENetAddress & addr, Client & client);
	virtual void update(float dt, Client & client);
	virtual void render(float t, Client & client);
	virtual void onExit(Client & client);
	virtual void onObscure(Client & client);
	virtual void onReveal(Client & client);
private:
	GameWorld m_gameWorld;
};