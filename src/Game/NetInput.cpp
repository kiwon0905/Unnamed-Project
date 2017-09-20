#include "NetInput.h"
#include "GameConfig.h"

void NetInput::write(Packer & packer)
{
	//packer.pack<0, MAX_TICK>(tick);
	packer.pack(*reinterpret_cast<int64_t*>(&aimDirection));
	packer.pack<-1, 1>(moveDirection);
	packer.pack<-1, 1>(vMoveDirection);
	packer.pack(jump);
	packer.pack(fire);
}

void NetInput::read(Unpacker & unpacker)
{
	//unpacker.unpack<0, MAX_TICK>(tick);
	unpacker.unpack(*reinterpret_cast<int64_t*>(&aimDirection));
	unpacker.unpack<-1, 1>(moveDirection);
	unpacker.unpack<-1, 1>(vMoveDirection);
	unpacker.unpack(jump);
	unpacker.unpack(fire);
}
