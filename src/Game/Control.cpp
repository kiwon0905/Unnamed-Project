#include "Control.h"

void NetInput::write(Packer & packer)
{
	packer.pack(*reinterpret_cast<int64_t*>(&aimDirection));
	packer.pack<-1, 1>(moveDirection);
	packer.pack(jump);
	packer.pack(fire);
}

void NetInput::read(Unpacker & unpacker)
{
	unpacker.unpack(*reinterpret_cast<int64_t*>(&aimDirection));
	unpacker.unpack<-1, 1>(moveDirection);
	unpacker.unpack(jump);
	unpacker.unpack(fire);
}
