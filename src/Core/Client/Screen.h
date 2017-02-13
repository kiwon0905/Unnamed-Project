#pragma once
#include <SFML/Window.hpp>
#include "Network.h"

class Client;

class Screen
{
public:
	Screen() = default;
	virtual ~Screen() = default;

	virtual void onEnter(Client & client) = 0;
	virtual void handleEvent(const sf::Event & ev, Client & client) = 0;
	virtual void handleNetEvent(ENetEvent & netEv, Client & client) = 0;
	virtual void handlePacket(Unpacker & unpacker, const ENetAddress & addr, Client & client) = 0;
	virtual void update(float dt, Client & client) = 0;
	virtual void render(float t, Client & client) = 0;
	virtual void onExit(Client & client) = 0;
	virtual void onObscure(Client & client) = 0;
	virtual void onReveal(Client & client) = 0;
};

