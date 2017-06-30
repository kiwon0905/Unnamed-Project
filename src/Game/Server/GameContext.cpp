#include "GameContext.h"
#include "Core/ENetUtility.h"
#include "Core/Logger.h"


GameContext::GameContext():
	m_gameWorld(this)
{
}

void GameContext::onMsg(Msg msg, Unpacker & unpacker, ENetPeer * enetPeer)
{
	if (msg == Msg::CL_REQUEST_JOIN_GAME)
	{
		Packer packer;
		if (m_state == PRE_GAME && m_peers.size() < MAX_PLAYER_ID + 1)
		{
			packer.pack(Msg::SV_ACCEPT_JOIN);
			Peer * p = new Peer(m_nextPeerId++, enetPeer);
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
		packer.pack<0, MAX_ENTITY_ID>(peer->getEntity()->getId());	//my entity id
		packer.pack(peer->getEntity()->getType());					//my entity type

		for (const auto & p : m_peers)
		{
			if (p->getId() != peer->getId())
			{
				packer.pack<0, MAX_PLAYER_ID>(p->getId());				//player id
				packer.pack<0, MAX_ENTITY_ID>(p->getEntity()->getId());	//entity id
			}
		}
		peer->send(packer, true);
	}
	else if (msg == Msg::CL_LOAD_COMPLETE && m_state == LOADING)
	{
		peer->setState(Peer::IN_GAME);
		Logger::getInstance().info(std::to_string(peer->getId()) + " has loaded");
		if (ensurePlayers(Peer::IN_GAME))
		{
			m_state = State::IN_GAME;
			Logger::getInstance().info("Everyone has loaded");
			m_clock.restart();
		}
	}
	else if (msg == Msg::CL_INPUT && m_state == IN_GAME)
	{
		NetInput input;
		input.read(unpacker);
		peer->onInput(input);
		if (m_tick % 2 == 0)
		{
			sf::Time timeLeft = sf::seconds(input.tick / TICKS_PER_SEC) - m_clock.getElapsedTime();

			Packer packer;
			packer.pack(Msg::SV_INPUT_TIMING);
			packer.pack<0, MAX_TICK>(input.tick);
			packer.pack(timeLeft.asMilliseconds());
			peer->send(packer, false);
		}
	}
}

void GameContext::onDisconnect(const ENetPeer & peer)
{
	Peer * p = getPeer(&peer);
	if (p)
		m_gameWorld.onDisconnect(*p);
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


			Packer packer;
			packer.pack(Msg::SV_SNAPSHOT);
			packer.pack<0, MAX_TICK>(m_tick);
			m_gameWorld.snap(packer);
			for (auto & p : m_peers)
			{
				p->send(packer, false);
				std::cout << "snap\n";
			}
		}

		checkRound();
	}
}

const std::vector<std::unique_ptr<Peer>>& GameContext::getPeers()
{
	return m_peers;
}

const Map & GameContext::getMap()
{
	return m_map;
}

int GameContext::getCurrentTick()
{
	return m_tick;
}

void GameContext::startRound()
{
	m_state = LOADING;

	m_map.loadFromFile("map/grass.xml");

	
	for (std::size_t i = 0; i < m_peers.size(); ++i)
	{
		auto & p = m_peers[i];

		if (i == 0)
		{
			Zombie * z = m_gameWorld.createEntity<Zombie>(p.get());
			p->setEntity(z);
		}
		else
		{
			Human * h = m_gameWorld.createEntity<Human>(p.get());
			p->setEntity(h);

		}
			
		
		Packer packer;
		packer.pack(Msg::SV_LOAD_GAME);
		p->send(packer, true);
	}
}

void GameContext::endRound(EntityType winner)
{
	if (winner == EntityType::NONE)
		std::cout << "DRAW!\n";
	else if (winner == EntityType::HUMAN)
		std::cout << "HUMAN WIN\n";
	else if (winner == EntityType::ZOMBIE)
		std::cout << "ZOMBIE WIN\n";

	Packer packer;
	packer.pack(Msg::SV_ROUND_OVER);
	packer.pack(winner);
	broadcast(packer, true);
	
	m_gameWorld.reset();
	m_state = PRE_GAME;
	m_tick = 0;
	m_prevTime = sf::Time::Zero;
	m_accumulator = sf::Time::Zero;
	for (auto & p : m_peers)
		p->setState(Peer::PRE_GAME);
}

void GameContext::checkRound()
{
	std::size_t humanCount = m_gameWorld.getEntities(EntityType::HUMAN).size();
	std::size_t zombieCount = m_gameWorld.getEntities(EntityType::ZOMBIE).size();

	EntityType winner = EntityType::NONE;
	//50 tick /s
	//5 min = 15000 tick

	//5 sec  = 250 tick
	if (m_tick > 250)
	{
		std::cout << "HUMAN: " << humanCount << " ZOMBIE: " << zombieCount << "\n";
		if (humanCount > 0)
		{
			winner = EntityType::HUMAN;
		}
		else if(humanCount == 0 && zombieCount > 0)
		{
			winner = EntityType::ZOMBIE;
		}
		endRound(winner);
	}
	/*
	else
	{
		if (humanCount > 0 && zombieCount == 0)
		{
			winner = EntityType::HUMAN;
		}
		else if (humanCount == 0 && zombieCount > 0)
		{
			winner = EntityType::ZOMBIE;
		}
		if (winner != EntityType::NONE)
			endRound(winner);
	}*/

}

Peer * GameContext::getPeer(const ENetPeer * peer)
{
	for (auto & p : m_peers)
		if (p->getENetPeer() == peer)
			return p.get();
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
