#include "GameWorld.h"
#include "Game/Snapshot.h"
#include "Core/Server/Server.h"
#include "Core/Protocol.h"
#include "Core/ENetUtility.h"
#include "Core/Logger.h"

#include "Human.h"
#include <iostream>
#include <bitset>

void GameWorld::onDisconnect(ENetPeer * peer)
{
	Player * p = getPlayer(peer);

	m_entities.erase(std::remove_if(m_entities.begin(), m_entities.end(), [p](const std::unique_ptr<Entity> & e) {return e.get() == p->getEntity(); }), m_entities.end());
	m_players.erase(std::remove_if(m_players.begin(), m_players.end(), [peer](Player & player) {return player.getPeer() == peer; }), m_players.end());
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
			Logger::getInstance().info("Accepted player: " + enutil::toString(peer->address));
			packer.pack(Msg::SV_ACCEPT_JOIN);
	
			Player player(peer);
			m_players.push_back(player);
		}
		else
		{
			Logger::getInstance().info("Rejected player: " + enutil::toString(peer->address));
			packer.pack(Msg::SV_REJECT_JOIN);
		}
		enutil::send(packer, peer, true);
	}
	else
	{
		Player * player = getPlayer(peer);
		if (!player)
			return;

		if (msg == Msg::CL_READY)
		{
			Logger::getInstance().info("Player is ready: " + enutil::toString(peer->address));
		
			Human * c = createCharacter(player);
			player->setEntity(c);
			player->setReady(true);
		}
		else if (msg == Msg::CL_INPUT)
		{
			std::uint8_t bits;
			unpacker.unpack(bits);
			player->getEntity()->onInput(bits);
		}
	}
}

void GameWorld::update(float dt)
{
	if (m_state == PRE && m_players.size() > 0)
	{
		bool ready = true;
		for (const Player & player : m_players)
		{
			if (!player.isReady())
			{
				ready = false;
				break;
			}
		}
		
		if (ready)
		{
			std::cout << "started game!\n";
			m_state = PER;
		}
	}
	if (m_state == PER)
	{
		for (const auto & entity : m_entities)
			entity->update(dt, *this);
	}
}

void GameWorld::sync()
{
	if (m_state == PER)
	{
		Packer packer;
		packer.pack(Msg::SV_SNAPSHOT);
		packer.pack<SNAPSHOT_SEQ_MIN,SNAPSHOT_SEQ_MAX>(m_nextSnapshotSeq);
		packer.pack<ENTITY_ID_MIN, ENTITY_ID_MAX + 1>(m_entities.size());
		for (std::size_t i = 0; i < m_entities.size(); ++i)
			m_entities[i]->snap(packer);
		for (Player & player : m_players)
			enutil::send(packer, player.getPeer(), false);

		m_nextSnapshotSeq++;

	}

}

void GameWorld::reset()
{
	m_reset = true;
}

Player * GameWorld::getPlayer(ENetPeer * peer)
{
	for (Player & player : m_players)
		if (player.getPeer() == peer)
			return &player;
	return nullptr;
}

Human * GameWorld::createCharacter(Player * player)
{
	Human * c = new Human(m_nextEntityId++, player);
	m_entities.emplace_back(c);
	return c;
}
