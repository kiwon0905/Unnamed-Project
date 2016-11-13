#pragma once

#include <SFML/System.hpp>


struct Snapshot
{
	sf::Uint32 seq;
	sf::Uint32 entityCount;
};
