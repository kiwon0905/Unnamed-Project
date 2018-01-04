#include "NetGameDataTdm.h"

NetObject::Type NetGameDataTdm::getType() const
{
	return NetObject::GAME_DATA_TDM;
}

void NetGameDataTdm::write(Packer & packer) const
{
	packer.pack(scoreA);
	packer.pack(scoreB);
}

void NetGameDataTdm::read(Unpacker & unpacker)
{
	unpacker.unpack(scoreA);
	unpacker.unpack(scoreB);
}
