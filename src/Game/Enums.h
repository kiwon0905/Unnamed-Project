#pragma once
#include <string>

enum class Team
{
	BLUE,
	RED,
	NONE,
	COUNT
};

enum class EntityType
{
	CHARACTER,
	ITEM,
	PROJECTILE,
	COUNT
};

enum class ProjectileType
{
	BULLET,
	ROCKET
};

inline std::string toString(Team t)
{
	switch (t)
	{
	case Team::BLUE:
		return "BLUE";
	case Team::RED:
		return "RED";
	case Team::NONE:
		return "NONE";
	default:
		return "";
	}
}