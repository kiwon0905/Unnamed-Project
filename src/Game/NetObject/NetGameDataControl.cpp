#include "NetGameDataControl.h"

NetObject::Type NetGameDataControl::getType() const
{
	return NetObject::GAME_DATA_CONTROL;
}

void NetGameDataControl::write(Packer & packer) const
{
	packer.pack(controlProgressA);
	packer.pack(controlProgressB);
	packer.pack(controllingTeam);

	packer.pack(captureProgressA);
	packer.pack(captureProgressB);
	packer.pack(capturingTeam);
}

void NetGameDataControl::read(Unpacker & unpacker)
{
	unpacker.unpack(controlProgressA);
	unpacker.unpack(controlProgressB);
	unpacker.unpack(controllingTeam);

	unpacker.unpack(captureProgressA);
	unpacker.unpack(captureProgressB);
	unpacker.unpack(capturingTeam);
}
