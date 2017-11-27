#pragma once
#include "Game/NetObject.h"

struct NetGameDataControl
{
	NetObject::Type getType() const;
	void write(Packer & packer) const;
	void read(Unpacker & unpacker);

	int controlProgressA;
	int controlProgressB;
	Team controllingTeam;

	int captureProgressA;
	int captureProgressB;
	Team capturingTeam;
};