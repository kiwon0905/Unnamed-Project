#pragma once

#include "Core/Packet.h"
#include <SFML/System.hpp>


struct Snapshot
{
	sf::Uint32 seq;
	sf::Uint32 entityCount;
};

Packet & operator<<(Packet & packet, const Snapshot & snapshot)
{
	packet << snapshot.entityCount;
	return packet;
}

Packet & operator >> (Packet & packet, Snapshot & snapshot)
{
	packet >> snapshot.entityCount;
	return packet;
}