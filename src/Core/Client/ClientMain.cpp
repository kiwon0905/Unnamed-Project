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

enum A
{
	First,a, b, c, d, LAST
};

int main()
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Packer writer;
	writer.pack(A::c);

	Unpacker reader(writer.getData(), writer.getDataSize());
	A a;
	reader.unpack(a);
	std::cout << (int)a << "\n";
	std::cin.get();
}