#include "Client.h"
#include "Core/Packer.h"

#ifdef _DEBUG
	#include <stdlib.h>
	#include <crtdbg.h>
	#define _CRTDBG_MAP_ALLOC
	#ifndef DBG_NEW
		#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
		#define new DBG_NEW
	#endif
#endif

#include "Core/Packer.h"

int main()
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Packer writer;
	writer.pack(std::int8_t(1));
	writer.pack(std::uint16_t(2));
	writer.pack(std::int32_t(1));
	writer.pack(false);
	writer.p();

	Unpacker reader(writer.getData(), writer.getDataSize());
	std::int8_t a;
	std::uint16_t b;
	std::int32_t c;
	reader.unpack(a);
	reader.unpack(b);
	reader.unpack(c);
	bool bo;
	reader.unpack(bo);
	std::cout << (int)a << " " << (int)b << " " << (int)c << "\n";
	std::cout << bo << "\n";
	std::cin.get();
}