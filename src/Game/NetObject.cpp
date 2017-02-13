#include "NetObject.h"
#include "GameCore.h"

void NetHuman::pack(Packer & packer)
{
	packer.pack<4>(0.f, 5000.f, position.x);
	packer.pack<4>(0.f, 5000.f, position.y);
}

void NetHuman::unpack(Unpacker & unpacker)
{
	unpacker.unpack<4>(0.f, 5000.f, position.x);
	unpacker.unpack<4>(0.f, 5000.f, position.y);
}

EntityType NetHuman::getType()
{
	return EntityType::HUMAN;
}
