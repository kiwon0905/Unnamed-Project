#pragma once

enum Control
{
	JUMP = 1 << 0,
	MOVE_LEFT = 1 << 1,
	MOVE_RIGHT = 1 << 2,
	MOVE_UP = 1 << 3,
	MOVE_DOWN = 1 << 4,
	PRIMARY_FIRE = 1 << 5,
	ABILITY1 = 1 << 6,
	ABILITY2 = 1 << 7,
};