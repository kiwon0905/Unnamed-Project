#pragma once
#include <string>

enum class Team
{
	A,
	B,
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
	case Team::A:
		return "A";
	case Team::B:
		return "B";
	case Team::NONE:
		return "NONE";
	default:
		return "";
	}
}