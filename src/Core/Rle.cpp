#include "Rle.h"


const std::size_t RUN_LENGTH = 3;


void encode(const void * data, std::size_t size, Packer & packer)
{
	Unpacker unpacker;
	unpacker.setData(data, size);

	std::size_t totalBits = 8 * size;
	std::size_t readBits = 0;
	

	while (readBits + RUN_LENGTH <= totalBits)
	{
		uint8_t bits;
		unpacker.unpack(bits, RUN_LENGTH);
		readBits += RUN_LENGTH;

		packer.pack(bits, RUN_LENGTH);


		if (bits == 0)
		{
			uint8_t next;
			unpacker.peek(next, RUN_LENGTH);
			std::size_t length = 1;
			while (next == 0 && length < (1 << RUN_LENGTH) - 1 && readBits + RUN_LENGTH <= totalBits)
			{
				++length;
				unpacker.unpack(next, RUN_LENGTH);
				unpacker.peek(next, RUN_LENGTH);
				readBits += RUN_LENGTH;

			}
			packer.pack(length, RUN_LENGTH);
		}
	}


	std::size_t remaining = totalBits - readBits;

	uint8_t r;
	unpacker.unpack(r, remaining);
	packer.pack(r, remaining);
	std::size_t validBits = packer.getCurrentBitPos();
	validBits = validBits == 0 ? 8 : validBits;

	packer.align();
	packer.pack(validBits, 8);
}

void decode(const void * data, std::size_t size, Packer & packer)
{
	Unpacker unpacker;
	unpacker.setData(data, size);

	unsigned validFinalBits = static_cast<const unsigned char*>(data)[size - 1];
	std::size_t totalBits = 8 * (size - 1) - (8 - validFinalBits);
	std::size_t readBits = 0;

	while (readBits + RUN_LENGTH <= totalBits)
	{
		uint8_t bits;
		unpacker.unpack(bits, RUN_LENGTH);
		readBits += RUN_LENGTH;
		if (bits == 0 && readBits + RUN_LENGTH <= totalBits)
		{
			uint8_t length;
			unpacker.unpack(length, RUN_LENGTH);
			readBits += RUN_LENGTH;
			for (std::size_t i = 0; i < length; ++i)
				packer.pack(0, RUN_LENGTH);

		}
		else
		{
			packer.pack(bits, RUN_LENGTH);
		
		}
	}
	std::size_t remaining = totalBits - readBits;
	std::uint8_t r;
	unpacker.unpack(r, remaining);
	packer.pack(r, remaining);
}
