#pragma once
#include <string>
enum class EntityType
{
	NONE,
	HUMAN,
	ZOMBIE,
	PROJECTILE,
	CRATE,
	ITEM,
	COUNT
};

enum class Team
{
	A,
	B,
	NONE,
	COUNT
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