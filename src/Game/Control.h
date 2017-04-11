#pragma once

enum Control
{
	JUMP = 1 << 0,
	MOVE_LEFT = 1 << 1,
	MOVE_RIGHT = 1 << 2,
	PRIMARY_FIRE = 1 << 3,
	ABILITY1 = 1 << 5,
	ABILITY2 = 1 << 6,
	ABILITY3 = 1 << 7,
};