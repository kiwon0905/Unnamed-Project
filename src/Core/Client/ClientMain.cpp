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



#include "Game/Snapshot.h"
#include "Core/Rle.h"
#include "Core/Utility.h"
int main()
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	Client client;
	client.run();

	/*

	
	float ratio = 0;
	const int TRIAL = 100;
	for (int i = 0; i < TRIAL; ++i)
	{
		Packer packer;
		for (int j = 0; j < 5; ++j)
		{
			if (Math::uniform(0, 3)() < 3)
			{
				packer.pack(std::uint8_t(0));
			}
			else
			{
				int n = Math::uniform(0, 255)();
				packer.pack(std::uint8_t(n));
			}

		}
		std::cout << "original: ";
		packer.p();


		Packer packer2;
		Unpacker unpacker(packer.getData(), packer.getDataSize());
		encode(unpacker, packer2);
		std::cout << "encoded: ";
		packer2.p();
		
		Packer packer3;
		Unpacker unpacker2(packer2.getData(), packer2.getDataSize());
		decode(unpacker2, packer3);
		std::cout << "decoded: ";

		packer3.p();
		std::cout << "\n";
		ratio += (float)packer2.getDataSize() / packer.getDataSize();
	}
	std::cout << "avg compress ratio: " << ratio / TRIAL << "\n";
	
	std::cin.get();
	*/
}