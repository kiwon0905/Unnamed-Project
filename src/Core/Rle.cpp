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
		unpacker.unpack8(bits, RUN_LENGTH);
		packer.pack8(bits, RUN_LENGTH);
		readBits += RUN_LENGTH;

		std::cout << "bits: " << (int)bits << "\n";
		if (bits == 0)
		{
			uint8_t next;
			unpacker.peek8(next, RUN_LENGTH);
			std::size_t length = 1;
			while (next == 0 && length < (1 << RUN_LENGTH) - 1 && readBits + RUN_LENGTH <= totalBits)
			{
				++length;
				unpacker.unpack8(next, RUN_LENGTH);
				unpacker.peek8(next, RUN_LENGTH);
				readBits += RUN_LENGTH;
			}
			packer.pack8(length, RUN_LENGTH);
			std::cout << "run length: " << length << "\n";
		}
	}


	std::size_t remaining = totalBits - readBits;
	std::cout << "remaining bits: " << remaining << "\n";
}

void decode(const void * data, std::size_t size, Packer & packer)
{
	Unpacker unpacker;
	unpacker.setData(data, size);
	std::size_t totalBits = 8 * size;
	std::size_t readBits = 0;

	while (readBits + RUN_LENGTH <= totalBits)
	{
		uint8_t bits;
		unpacker.unpack8(bits, RUN_LENGTH);
		std::cout << "bits: " << (int)bits << "\n";
		readBits += RUN_LENGTH;
		if (bits == 0)
		{
			uint8_t length;
			unpacker.unpack8(length, RUN_LENGTH);
			readBits += RUN_LENGTH;
			for (std::size_t i = 0; i < length; ++i)
				packer.pack8(0, RUN_LENGTH);
		}
		else
		{
			packer.pack8(bits, RUN_LENGTH);
		}
	}
}
