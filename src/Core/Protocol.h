#pragma once

enum class Msg
{
	//client -> masterserver 
	CL_REQUEST_INTERNET_SERVER_LIST,
	//client -> lan
	CL_REVEAL_LAN_SERVER,

	//client -> server
	CL_REQUEST_JOIN_GAME,
	CL_READY,
	CL_INPUT,



	//server -> masterserver
	SV_REGISTER_SERVER,

	//server -> client
	SV_LAN_GAME_LIST,

	//server -> client
	SV_ACCEPT_JOIN,
	SV_REJECT_JOIN,
	SV_SNAPSHOT,

	//master -> client
	MSV_INTERNET_SERVER_LIST,

	INVALID,
	LAST
};

//Master server 
//host - 12345		masterserver <-> gameserver
//socket - 12344	masterserver <-> client

//Game server
//host - 12346		gameserver <-> client
//socket - 12347	gameserver <-> client