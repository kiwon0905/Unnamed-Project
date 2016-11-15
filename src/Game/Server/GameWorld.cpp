#include "GameWorld.h"
#include "Game/Snapshot.h"
#include "Core/Server/Server.h"
#include "Core/Protocol.h"
#include "Core/ENetUtility.h"

#include <iostream>
#include <bitset>
void GameWorld::onDisconnect(ENetPeer * peer)
{
	if (!m_peerPlayers.count(peer))
		return;

	Entity * playerEntity = m_peerPlayers[peer]->getEntity();
	m_players.erase(std::remove_if(m_players.begin(), m_players.end(), [peer](Player & player) {return player.getPeer() == peer; }), m_players.end());
	m_peerPlayers.erase(peer);
	m_entities.erase(std::remove_if(m_entities.begin(), m_entities.end(), [playerEntity](const std::unique_ptr<Entity> & e) {return e.get() == playerEntity; }), m_entities.end());
}

void GameWorld::handlePacket(Unpacker & unpacker, ENetPeer * peer)
{
	Msg msg;
	unpacker.unpack(msg);

	if (msg == Msg::CL_REQUEST_JOIN_GAME)
	{
		Packer packer;
		if (m_players.size() < 3)
		{
			packer.pack(Msg::SV_ACCEPT_JOIN);
	
			Player player(peer);
			m_players.push_back(player);
			m_peerPlayers[peer] = &m_players.back();
		}
		else
		{
			packer.pack(Msg::SV_REJECT_JOIN);
		}
		enutil::send(packer, peer, true);
	}
	else if (msg == Msg::CL_READY)
	{
		Entity * e = createEntity(m_peerPlayers[peer]);
		m_peerPlayers[peer]->setEntity(e);
	}
	else if (msg == Msg::CL_INPUT)
	{
		std::uint8_t bits;
		unpacker.unpack(bits);
		std::cout << std::bitset<8>(bits) << "\n";
	}
}

void GameWorld::tick(float dt)
{
}

void GameWorld::sync()
{


}

void GameWorld::reset()
{
	m_reset = true;
}

Entity * GameWorld::createEntity(Player * player)
{
	Entity * e = new Entity(m_nextEntityId++, player);
	m_entities.emplace_back(e);
	return e;
}
