#pragma once
#include "Game/NetObject.h"

struct NetGameDataTdm
{
	NetObject::Type getType() const;
	void write(Packer & packer) const;
	void read(Unpacker & unpacker);

	int scoreA;
	int scoreB;
};