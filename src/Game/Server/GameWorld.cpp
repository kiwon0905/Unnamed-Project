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
			Human * c = createCharacter(&m_players.back());
			m_players.back().setEntity(c);
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

		if (msg == Msg::CL_REQUEST_WORLD_INFO)
		{
			Packer packer;
			packer.pack(Msg::SV_WORLD_INFO);
			packer.pack<ENTITY_ID_MIN, ENTITY_ID_MAX>(player->getEntity()->getId());
			enutil::send(packer, peer, true);
			std::cout << "cl_request_world\n";
		}
		else if (msg == Msg::CL_READY)
		{
			Logger::getInstance().info("Player is ready: " + enutil::toString(peer->address));
		
			player->setReady(true);
		}
		else if (msg == Msg::CL_INPUT)
		{
			unsigned seq;
			std::uint8_t bits;
			unpacker.unpack<INPUT_SEQ_MIN, INPUT_SEQ_MAX>(seq);
			unpacker.unpack(bits);
			player->getEntity()->onInput(bits);
			player->setInputSeq(seq);
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
			m_state = IN_PROGRESS;
		}
	}
	if (m_state == IN_PROGRESS)
	{
		for (const auto & entity : m_entities)
			entity->update(dt, *this);
	}
}

void GameWorld::sync()
{
	if (m_state == IN_PROGRESS)
	{
		for (Player & player : m_players)
		{
			Packer packer;
			packer.pack(Msg::SV_SNAPSHOT);
			packer.pack<SNAPSHOT_SEQ_MIN,SNAPSHOT_SEQ_MAX>(m_nextSnapshotSeq);
			packer.pack<INPUT_SEQ_MIN, INPUT_SEQ_MAX>(player.getInputSeq());
			packer.pack<ENTITY_ID_MIN, ENTITY_ID_MAX + 1>(m_entities.size());
			for (std::size_t i = 0; i < m_entities.size(); ++i)
				m_entities[i]->snap(packer);
			for (Player & player : m_players)
				enutil::send(packer, player.getPeer(), false);

		}
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
