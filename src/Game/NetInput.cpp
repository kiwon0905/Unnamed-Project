#include "NetInput.h"
#include "GameConfig.h"

void NetInput::write(Packer & packer)
{
	packer.pack(*reinterpret_cast<int64_t*>(&aimDirection));
	packer.pack(moveDirection);
	packer.pack(jump);
	packer.pack(fire);
}

void NetInput::read(Unpacker & unpacker)
{
	unpacker.unpack(*reinterpret_cast<int64_t*>(&aimDirection));
	unpacker.unpack(moveDirection);
	unpacker.unpack(jump);
	unpacker.unpack(fire);
}
