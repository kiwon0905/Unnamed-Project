#pragma once

#include "Screen.h"

#include <stack>
#include <memory>
#include <vector>

class Client;
class Packet;

class ScreenStack
{
public:
	bool initialize(Client & client);
	void finalize(Client & client);
	void update(Client & client);
	void render(Client & client);


	void pop();
	void push(Screen * screen);
	void clear();
	bool isEmpty();
	
	void handleEvent(const sf::Event & ev, Client & client);
	void handleNetEvent(ENetEvent & netEv, Client & client);
	void handlePacket(Unpacker & unpacker, const ENetAddress & addr, Client & client);
	void applyChanges(Client & client);
private:
	struct Change
	{
		int action;
		Screen * screen;
	};
	std::stack<std::unique_ptr<Screen>> m_screens;
	std::vector<Change> m_changes;
};