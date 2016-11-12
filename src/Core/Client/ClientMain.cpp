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

	//Client client;
	//client.run();

	Packer writer;
	for (int i = 0; i < 100; ++i)
	{
		writer.pack<0, 100>(i);
	}

	writer.pack("Hello!");
	writer.pack("wtf");
	writer.pack("Ayy");

	for (int i = -8; i <= 8; i += 2)
	{
		writer.pack<-10, 10, 2>(float(i) + i / 100.f);
	}


	std::cout << "Total size: " << writer.getDataSize() << "\n";
	Unpacker reader(writer.getData(), writer.getDataSize());
	for (int i = 0; i < 100; ++i)
	{
		int32_t val;
		reader.unpack<0, 100>(val);
		std::cout << val << "\n";
	}

	std::string out;
	reader.unpack(out);
	std::cout << out << "\n";
	reader.unpack(out);
	std::cout << out << "\n";
	reader.unpack(out);
	std::cout << out << "\n";

	for (int i = -8; i <= 8; i += 2)
	{
		float val;
		reader.unpack<-10, 10, 2>(val);
		std::cout << val << "\n";
	}

	std::cin.get();
}