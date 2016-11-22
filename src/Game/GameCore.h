#pragma once

const unsigned int ENTITY_ID_MIN = 0;	//0 is an invalid entity id
const unsigned int ENTITY_ID_MAX = 10000; //10 Entity/sec * 60 *5=3000
const unsigned int SNAPSHOT_SEQ_MIN = 0;
const unsigned int SNAPSHOT_SEQ_MAX = 10000; //5 min game: 20 snapshots/sec * 60 * 5 = 6000
const unsigned int INPUT_SEQ_MIN = 0; //0 is an invalid input sequence
const unsigned int INPUT_SEQ_MAX = 50000; //60/second = 3600/min = 18000 / 5 min
enum EntityType
{
	HUMAN,
	ZOMBIE,
	COUNT
};
enum EntityState
{

};