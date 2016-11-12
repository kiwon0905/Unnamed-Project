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

void GameWorld::handlePacket(Packet & packet, ENetPeer * peer)
{
	Msg msg;
	packet >> msg;

	if (msg == Msg::CL_REQUEST_JOIN_GAME)
	{
		Packet replyPacket;
		if (m_players.size() < 3)
		{
			replyPacket << Msg::SV_ACCEPT_JOIN;
	
			Player player(peer);
			m_players.push_back(player);
			m_peerPlayers[peer] = &m_players.back();
		}
		else
		{
			replyPacket << Msg::SV_REJECT_JOIN;
		}
		enutil::send(peer, replyPacket, true);
	}
	else if (msg == Msg::CL_READY)
	{
		Entity * e = createEntity(m_peerPlayers[peer]);
		m_peerPlayers[peer]->setEntity(e);
	}
	else if (msg == Msg::CL_INPUT)
	{
		sf::Uint8 bits;
		packet >> bits;
		if (m_peerPlayers.count(peer))
			m_peerPlayers[peer]->setInput(bits);
	}
}

void GameWorld::tick(float dt)
{
}

void GameWorld::sync()
{
	Snapshot ss;
	ss.seq = m_nextSnapshotSeq++;
	ss.entityCount = m_entities.size();
	Packet packet;
	packet << Msg::SV_SNAPSHOT << ss;
	for (int i = 0; i < 100; ++i)
	{
		int x = rand() % 100 + 1;
		int y = rand() % 100 + 1;
		int c = rand() % 100 + 1;
		packet << x << y << sf::Uint8(c);
	}
	for (auto & player : m_players)
		enutil::send(player.getPeer(), packet, false);
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
