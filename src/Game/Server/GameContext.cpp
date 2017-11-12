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



		//if (m_tick % 2 == 0)
		{

			std::unique_ptr<Snapshot> snapshot = std::make_unique<Snapshot>();
			for (auto & p : m_server->getPeers())
				p->snap(*snapshot);
			m_gameWorld.snap(*snapshot);



			for (auto & p : m_server->getPeers())
			{
				Packer packer;
				Snapshot * delta = m_snapshots.get(p->getAckTick());
				delta = 0;
				if (delta)
				{
					packer.pack_v(Msg::SV_DELTA_SNAPSHOT);
					packer.pack_v(m_tick);
					packer.pack_v(p->getAckTick());
					snapshot->writeRelativeTo(packer, *delta);
					
					Packer packer2;
					packer2.pack_v(Msg::SV_FULL_SNAPSHOT);
					packer2.pack_v(m_tick);
					snapshot->write(packer2);
				}
				else
				{
					packer.pack_v(Msg::SV_FULL_SNAPSHOT);
					packer.pack_v(m_tick);
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
			packer.pack_v(Msg::SV_ROUND_OVER);
			packer.pack_v(winner);
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

void GameContext::announceDeath(int killedPeer, int killerPeer, const std::vector<int> & assisters)
{
	Peer * killed = m_server->getPeer(killedPeer);
	Peer * killer = m_server->getPeer(killerPeer);

	Packer packer;
	packer.pack_v(Msg::SV_KILL_FEED);
	std::cout << "killed at: " << m_tick << ". Assisters: ";
	for (int i : assisters)
		std::cout << i << ", ";
	std::cout << "\n";
	//necessary??
	int killedPeerId = killed ? killed->getId() : -1;
	int killerPeerId = killer ? killer->getId() : -1;
	packer.pack_v(killedPeerId);
	packer.pack_v(killerPeerId);
	m_server->broadcast(packer, true);
}

void GameContext::addScore(int peerId, int score)
{
	Peer * peer = m_server->getPeer(peerId);
	if (peer)
	{
		peer->addScore(score);
	}
}
