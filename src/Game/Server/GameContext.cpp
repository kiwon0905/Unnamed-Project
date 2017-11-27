#include "GameContext.h"
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
		for (auto & p : m_players)
			p.tick();
		tick(sf::seconds(1.f / TICKS_PER_SEC).asSeconds());

		//if (m_tick % 2 == 0)
		{

			std::unique_ptr<Snapshot> snapshot = std::make_unique<Snapshot>();
			for (auto & p : m_players)
				p.snap(*snapshot);
			m_gameWorld.snap(*snapshot);
			snap(*snapshot);

			for (auto & p : m_players)
			{
				Packer packer;
				Snapshot * delta = m_snapshots.get(p.getAckTick());
				if (delta)
				{
					packer.pack(Msg::SV_DELTA_SNAPSHOT);
					packer.pack(m_tick);
					packer.pack(p.getAckTick());
					snapshot->writeRelativeTo(packer, *delta);
				}
				else
				{
					packer.pack(Msg::SV_FULL_SNAPSHOT);
					packer.pack(m_tick);
					snapshot->write(packer);
				}
				m_server->getPeer(p.getPeerId())->send(packer, false);
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

void GameContext::onJoin(int peerId)
{
	m_players.emplace_back(peerId, this);
}

void GameContext::onDisconnect(int peerId)
{
	m_gameWorld.onDisconnect(*getPlayer(peerId));
	
	auto isDead = [peerId](Player & p) {return p.getPeerId() == peerId; };

	m_players.erase(std::remove_if(m_players.begin(), m_players.end(), isDead), m_players.end());
}

void GameContext::onInput(int peerId, int predictedTick, const NetInput & input, int ackTick)
{
	Player * player = getPlayer(peerId);
	player->onInput(predictedTick, input);
	player->setAckTick(ackTick);

	if(m_tick % 2)
	{
		sf::Time timeLeft = sf::seconds(predictedTick / TICKS_PER_SEC) - m_clock.getElapsedTime();
		Packer packer;
		packer.pack(Msg::SV_INPUT_TIMING);
		packer.pack(predictedTick);
		packer.pack(timeLeft.asMilliseconds());
		m_server->getPeer(peerId)->send(packer, false);
	}
}

int GameContext::getCurrentTick() const
{
	return m_tick;
}

sf::Time GameContext::getCurrentTime() const
{
	return m_clock.getElapsedTime();
}

void GameContext::loadRound()
{
	m_map.loadFromTmx("map/control0.tmx");
}

void GameContext::startRound()
{
	onRoundStart();
	m_clock.restart();
}

void GameContext::reset()
{
	m_gameWorld.reset();
	m_tick = 0;
	m_prevTime = sf::Time::Zero;
	m_accumulator = sf::Time::Zero;
	for (auto & p : m_players)
		p.reset();
	m_snapshots.clear();
}

void GameContext::announceDeath(int killedPeer, int killerPeer, const std::unordered_map<int, int> & assisters)
{
	Player * killed = getPlayer(killedPeer);
	Player * killer = getPlayer(killerPeer);
	if (killed)
		killed->addDeath();
	if (killer)
		killer->addKill();
	Packer packer;
	packer.pack(Msg::SV_KILL_FEED);
	for (auto & p : assisters)
	{
		Player * assister = getPlayer(p.first);
		if (assister)
			assister->addAssist();
	}
	
	//necessary??
	int killedPeerId = killed ? killed->getPeerId() : -1;
	int killerPeerId = killer ? killer->getPeerId() : -1;
	packer.pack(killedPeerId);
	packer.pack(killerPeerId);
	m_server->broadcast(packer, true);
}

void GameContext::addScore(int peerId, int score)
{
	Player * p = getPlayer(peerId);
	if (p)
		p->addScore(score);

}

Player * GameContext::getPlayer(int peerId)
{
	for (auto & p : m_players)
		if (p.getPeerId() == peerId)
			return &p;
	return nullptr;
}
