#include "Rle.h"


const std::size_t RUN_LENGTH = 3;


void encode(Unpacker & unpacker, Packer & packer)
{
	Packer temp;
	while (unpacker.getRemainingBits() >= RUN_LENGTH)
	{
		int count = 0;
		unsigned bits = 0;
		do
		{
			unpacker.unpack(bits, RUN_LENGTH);
			count++;
		} while (bits == 0 && unpacker.getRemainingBits() >= RUN_LENGTH && count < (1 << RUN_LENGTH));

		if (count > 1)
		{
			unpacker.ununpack(RUN_LENGTH);
			temp.pack(0, RUN_LENGTH);
			temp.pack(count - 1, RUN_LENGTH);
		}
		else
		{
			temp.pack(bits, RUN_LENGTH);
		}
	}

	int remaining = unpacker.getRemainingBits();
	if (remaining)
	{
		int a;
		unpacker.unpack(a, remaining);
		temp.pack(a, remaining);
	}
	std::size_t finalValidBits = temp.getCurrentBitPos();
	packer.pack(finalValidBits, 7);
	packer.pack(temp.getData(), temp.getDataSize(), true);
}

void decode(Unpacker & unpacker, Packer & packer)
{
	int finalValidBits;
	unpacker.unpack(finalValidBits, 7);
	unpacker.align();
	if (finalValidBits == 0)
		finalValidBits = 8;
	while (unpacker.getRemainingBits() - 8 + finalValidBits >= RUN_LENGTH)
	{
		unsigned bits;
		unpacker.unpack(bits, RUN_LENGTH);

		if (bits == 0)
		{
			packer.pack(0, RUN_LENGTH);
			if (unpacker.getRemainingBits() - 8 + finalValidBits >= RUN_LENGTH)
			{
				int count;
				unpacker.unpack(count, RUN_LENGTH);
				for (int i = 0; i < count - 1; ++i)
				{
					packer.pack(0, RUN_LENGTH);
				}
			}
		}
		else
		{
			packer.pack(bits, RUN_LENGTH);
		}
	}

	int remaining = unpacker.getRemainingBits() - 8 + finalValidBits;
	if (remaining)
	{
		unsigned r;
		unpacker.unpack(r, remaining);
		packer.pack(r, remaining);
	}
}
