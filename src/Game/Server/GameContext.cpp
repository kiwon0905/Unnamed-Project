#include "GameContext.h"
#include "Game/Server/Entity/Crate.h"
#include "Core/Server/Server.h"
#include "Core/ENetUtility.h"
#include "Core/Logger.h"
#include "Core/Rle.h"


GameContext::GameContext(Server * server):
	m_server(server),
	m_gameWorld(this)
{
}

bool GameContext::update()
{

	sf::Time current = m_clock.getElapsedTime();
	sf::Time dt = current - m_prevTime;
	m_prevTime = current;
	m_accumulator += dt;

	while (m_accumulator >= sf::seconds(1.f / TICKS_PER_SEC))
	{
		m_tick++;
		m_accumulator -= sf::seconds(1.f / TICKS_PER_SEC);
		m_gameWorld.tick();



		if (m_tick % 2 == 0)
		{

			std::unique_ptr<Snapshot> snapshot = std::make_unique<Snapshot>();
			m_gameWorld.snap(*snapshot);



			for (auto & p : m_server->getPeers())
			{
				Packer packer;
				Snapshot * delta = m_snapshots.get(p->getAckTick());
				//delta = 0;
				if (delta)
				{
					packer.pack(Msg::SV_DELTA_SNAPSHOT);
					packer.pack<-1, MAX_TICK>(m_tick);
					packer.pack<-1, MAX_TICK>(p->getAckTick());
					snapshot->writeRelativeTo(packer, *delta);
				}
				else
				{
					packer.pack(Msg::SV_FULL_SNAPSHOT);
					packer.pack<-1, MAX_TICK>(m_tick);
					snapshot->write(packer);
				}
				p->send(packer, false);
			}

			m_snapshots.add(snapshot.release(), m_tick);
			m_snapshots.removeUntil(static_cast<int>(m_tick - TICKS_PER_SEC * 3));
		}

		Team winner;
		if (checkRound(winner))
		{
			if (winner == Team::A)
				std::cout << "TEAM A WIN!\n";
			else if (winner == Team::B)
				std::cout << "TEAM B WIN\n";
			else if (winner == Team::NONE)
				std::cout << "DRAW\n";

			Packer packer;
			packer.pack(Msg::SV_ROUND_OVER);
			packer.pack(winner);
			m_server->broadcast(packer, true);

			return false;
		}


	}
		
	return true;
}

Server * GameContext::getServer()
{
	return m_server;
}

const Map & GameContext::getMap() const
{
	return m_map;
}

GameWorld & GameContext::getWorld()
{
	return m_gameWorld;
}

int GameContext::getCurrentTick() const
{
	return m_tick;
}

sf::Time GameContext::getCurrentTime() const
{
	return m_clock.getElapsedTime();
}

void GameContext::createCrates()
{
	for (const auto & v : m_map.getCratePositions())
	{
		m_gameWorld.createEntity<Crate>(v);
	}
}

void GameContext::startRound()
{
	m_clock.restart();
}

void GameContext::reset()
{
	m_gameWorld.reset();
	m_tick = 0;
	m_prevTime = sf::Time::Zero;
	m_accumulator = sf::Time::Zero;

	m_snapshots.clear();
}

void GameContext::announceDeath(int killedEntity, int killerEntity)
{
	Peer * killedPeer = m_server->getPeerByEntityId(killedEntity);
	Peer * killerPeer = m_server->getPeerByEntityId(killerEntity);

	Packer packer;
	packer.pack(Msg::SV_KILL_FEED);
	int killedPeerId = killedPeer ? killedPeer->getId() : -1;
	int killerPeerId = killerPeer ? killerPeer->getId() : -1;
	packer.pack<-1, MAX_PEER_ID>(killedPeerId);
	packer.pack<-1, MAX_PEER_ID>(killerPeerId);
	m_server->broadcast(packer, true);
}
