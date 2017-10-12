#include "GameContext.h"
#include "Game/Server/Entity/Crate.h"
#include "Core/ENetUtility.h"
#include "Core/Logger.h"
#include "Core/Rle.h"


GameContext::GameContext():
	m_gameWorld(this)
{
}

void GameContext::onMsg(Msg msg, Unpacker & unpacker, ENetPeer * enetPeer)
{
	if (msg == Msg::CL_REQUEST_JOIN_GAME)
	{
		std::string name;
		unpacker.unpack(name);
		Packer packer;
		if (m_state == PRE_GAME && m_peers.size() < MAX_PLAYER_ID + 1)
		{
			packer.pack(Msg::SV_ACCEPT_JOIN);
			Peer * p = new Peer(m_nextPeerId++, enetPeer);
			p->setName(name);
			m_peers.emplace_back(p);
		}
		else
		{
			packer.pack(Msg::SV_REJECT_JOIN);
		}
		enutil::send(packer, enetPeer, true);
	}

	Peer * peer = getPeer(enetPeer);
	if (!peer)
		return;


	if (msg == Msg::CL_REQUEST_ROOM_INFO)
	{

	}
	else if (msg == Msg::CL_REQUEST_GAME_INFO && m_state == LOADING)
	{
		peer->setState(Peer::LOADING);

		Packer packer;
		packer.pack(Msg::SV_GAME_INFO);
		packer.pack(m_map.getName());
		packer.pack<0, MAX_PLAYER_ID>(m_peers.size());
		packer.pack<0, MAX_PLAYER_ID>(peer->getId());				//my player id
		packer.pack(peer->getName());								//my name
		packer.pack(peer->getTeam());								//team
		packer.pack<0, MAX_ENTITY_ID>(peer->getEntity()->getId());	//my entity id
		packer.pack(peer->getEntity()->getType());					//my entity type

		for (const auto & p : m_peers)
		{
			if (p->getId() != peer->getId())
			{
				packer.pack<0, MAX_PLAYER_ID>(p->getId());				//player id
				packer.pack(p->getName());							//name
				packer.pack(p->getTeam());								//team
				packer.pack<0, MAX_ENTITY_ID>(p->getEntity()->getId());	//entity id
			}
		}
		peer->send(packer, true);
	}
	else if (msg == Msg::CL_LOAD_COMPLETE && m_state == LOADING)
	{
		peer->setState(Peer::IN_GAME);
		Logger::getInstance().info("GameContext", std::to_string(peer->getId()) + " has loaded");
		if (ensurePlayers(Peer::IN_GAME))
		{
			m_state = State::IN_GAME;
			Logger::getInstance().info("GameContext", "Everyone has loaded");
			m_clock.restart();
		}
	}
	else if (msg == Msg::CL_INPUT && m_state == IN_GAME)
	{
		int predictedTick;
		int ackTick;
		unpacker.unpack<-1, MAX_TICK>(predictedTick);
		unpacker.unpack<-1, MAX_TICK>(ackTick);
		NetInput input;
		input.read(unpacker);
		peer->onInput(predictedTick, input);
		peer->setAckTick(ackTick);
		if (m_tick % 2 == 0)
		{
			sf::Time timeLeft = sf::seconds(predictedTick / TICKS_PER_SEC) - m_clock.getElapsedTime();

			Packer packer;
			packer.pack(Msg::SV_INPUT_TIMING);
			packer.pack<-1, MAX_TICK>(predictedTick);
			packer.pack(timeLeft.asMilliseconds());
			peer->send(packer, false);
		}
	}
	else if (msg == Msg::CL_CHAT)
	{
		std::string chat;
		unpacker.unpack(chat);
		
		Packer packer;
		packer.pack(Msg::SV_CHAT);
		packer.pack(uint8_t(peer->getId()));
		packer.pack(chat);
		broadcast(packer, true);
	}
}

void GameContext::onDisconnect(const ENetPeer & peer)
{
	Peer * p = getPeer(&peer);
	if (p)
		m_gameWorld.onDisconnect(*p);
	auto pred = [&peer](const auto & ptr)
	{
		return ptr->getENetPeer() == &peer;
	};
	m_peers.erase(std::remove_if(m_peers.begin(), m_peers.end(), pred), m_peers.end());

	if (m_peers.empty())
		reset();
}

void GameContext::update()
{
	if (m_state == PRE_GAME)
	{

	}
	else if (m_state == LOADING)
	{
	//TODO: loading timeouts, etc...
	}
	else if (m_state == IN_GAME)
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



				for (auto & p : m_peers)
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


		
			checkRound();
		
		}

	}
}

const std::vector<std::unique_ptr<Peer>>& GameContext::getPeers()
{
	return m_peers;
}

Peer * GameContext::getPeer(int id)
{
	for (auto & p : m_peers)
		if (p->getId() == id)
			return p.get();
	return nullptr;
}

const Map & GameContext::getMap()
{
	return m_map;
}

GameWorld & GameContext::getWorld()
{
	return m_gameWorld;
}

int GameContext::getCurrentTick()
{
	return m_tick;
}

void GameContext::createCrates()
{
	for (const auto & v : m_map.getCratePositions())
	{
		m_gameWorld.createEntity<Crate>(v);
	}
}

void GameContext::endRound(Team winner)
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
	broadcast(packer, true);
	reset();
}

void GameContext::reset()
{
	Logger::getInstance().info("GameContext", "server reset!");
	m_gameWorld.reset();
	m_state = PRE_GAME;
	m_tick = 0;
	m_prevTime = sf::Time::Zero;
	m_accumulator = sf::Time::Zero;
	for (auto & p : m_peers)
	{
		p->setState(Peer::PRE_GAME);
		p->setEntity(nullptr);
	}
	m_nextPeerId = 0;
	m_snapshots.clear();
}

void GameContext::announceDeath(int killedEntity, int killerEntity)
{
	Peer * killedPeer = getPeerByEntityId(killedEntity);
	Peer * killerPeer = getPeerByEntityId(killerEntity);

	Packer packer;
	packer.pack(Msg::SV_KILL_FEED);
	int killedPeerId = killedPeer ? killedPeer->getId() : -1;
	int killerPeerId = killerPeer ? killerPeer->getId() : -1;
	packer.pack<-1, MAX_PLAYER_ID>(killedPeerId);
	packer.pack<-1, MAX_PLAYER_ID>(killerPeerId);
	broadcast(packer, true);
}

Peer * GameContext::getPeer(const ENetPeer * peer)
{
	for (auto & p : m_peers)
		if (p->getENetPeer() == peer)
			return p.get();
	return nullptr;
}

Peer * GameContext::getPeerByEntityId(int id)
{
	for (const auto & p : m_peers)
	{
		if (p->getEntity() && p->getEntity()->getId() == id)
			return p.get();
	}
	return nullptr;
}

bool GameContext::ensurePlayers(Peer::State state)
{
	if (m_peers.empty())
		return false;
	for (auto & p : m_peers)
	{
		if (p->getState() != state)
			return false;
	}
	return true;
}

void GameContext::broadcast(const Packer & packer, bool reliable, const Peer * exclude)
{
	for (auto & p : m_peers)
	{
		if (p.get() != exclude)
		{
			p->send(packer, reliable);
		}
	}
}
