#pragma once

const int ENTITY_ID_MIN = 0;	
const int ENTITY_ID_MAX = 100000; //10 Entity/sec * 60 * 2 = 72,000
const int TICK_MIN = 0;
const int TICK_MAX = 500000;
const int INPUT_SEQ_MIN = 0; 
const int INPUT_SEQ_MAX = 500000; // 120 min game: 60 input/sec = 432,000 input/2h
const float GAME_TIME_MIN = 0.f;
const float GAME_TIME_MAX = 10000.f; //2h = 7200sec
const int SERVER_TICK_RATE = 60;
enum EntityType
{
	HUMAN,
	ZOMBIE,
	COUNT
};
enum EntityState
{

};