#include "NetInput.h"
#include "GameConfig.h"

void NetInput::write(Packer & packer)
{
	packer.pack_v(*reinterpret_cast<int64_t*>(&aimDirection));
	packer.pack_v(moveDirection);
	packer.pack_v(jump);
	packer.pack_v(fire);
}

void NetInput::read(Unpacker & unpacker)
{
	unpacker.unpack_v(*reinterpret_cast<int64_t*>(&aimDirection));
	unpacker.unpack_v(moveDirection);
	unpacker.unpack_v(jump);
	unpacker.unpack_v(fire);
}
