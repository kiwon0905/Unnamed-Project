#include "Client.h"

#ifdef _DEBUG
	#include <stdlib.h>
	#include <crtdbg.h>
	#define _CRTDBG_MAP_ALLOC
	#ifndef DBG_NEW
		#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
		#define new DBG_NEW
	#endif
#endif

/*
#include "Core/Utility.h"
#include "Core/Client/Particles.h"
int main()
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	Client client;
	client.run();
}*/



#include "Core/Rle.h"
int main()
{
	Packer packer;
	for (int i = 0; i < 5; ++i)
		packer.pack8(0, 3);

	packer.pack8(7, 3);
	for (int i = 0; i < 5; ++i)
		packer.pack8(0, 3);
	packer.pack8(3, 3);

	for (int i = 0; i < 3; ++i)
		packer.pack8(0, 3);
	packer.pack8(1, 3);
	packer.p();

	Packer packer2;
	encode(packer.getData(), packer.getDataSize(), packer2);
	packer2.p();

	Packer packer3;
	decode(packer2.getData(), packer2.getDataSize(), packer3);
	packer3.p();
	std::cin.get();
	return 0;
}