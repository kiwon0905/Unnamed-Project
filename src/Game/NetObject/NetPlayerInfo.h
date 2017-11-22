#pragma once
#include "Game/NetObject.h"

struct NetPlayerInfo
{
	NetObject::Type getType() const;
	void write(Packer & packer) const;
	void read(Unpacker & unpacker);

	NetObject::Type type;
	int id;
	Team team;
	int ping;
	int score;
	int kills;
	int deaths;
	int assists;
	int respawnTick;

};