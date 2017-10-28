#pragma once

enum class Msg
{
	//client -> masterserver 
	CL_REQUEST_INTERNET_SERVER_INFO,
	//client -> lan
	CL_REQUEST_LAN_SERVER_INFO,

	//client -> server
	CL_PING,
	CL_REQUEST_JOIN_GAME,
	CL_CLIENT_INFO,
	CL_REQUEST_ROOM_INFO,
	CL_REQUEST_PLAYER_INFO,
	CL_REQUEST_GAME_INFO,
	CL_LOAD_COMPLETE,
	CL_READY,
	CL_INPUT,
	CL_CHAT,

	//server -> masterserver
	SV_REGISTER_SERVER,
	SV_SERVER_INFO,
	//server -> client
	SV_LAN_SERVER_INFO,
	
	//server -> client
	SV_PING_REPLY,
	SV_ACCEPT_JOIN,
	SV_REJECT_JOIN,
	SV_LOAD_GAME,
	SV_GAME_INFO,
	SV_FULL_SNAPSHOT,
	SV_DELTA_SNAPSHOT,
	SV_INPUT_TIMING,
	SV_ROUND_OVER,
	SV_CHAT,
	SV_PLAYER_INFO,
	SV_PLAYER_LEFT,
	//game related
	SV_KILL_FEED,

	//master -> client
	MSV_INTERNET_SERVER_INFO,

	INVALID,
	COUNT
};

//Master server 
//host - 12345		masterserver <-> gameserver
//socket - 12344	masterserver <-> client

//Game server
//host - 12346		gameserver <-> client
//socket - 12347	gameserver <-> client