#include "GameWorld.h"
#include "Core/Server/Server.h"
#include "Core/Protocol.h"
#include "Core/ENetUtility.h"
#include "Core/Logger.h"

#include "Human.h"
#include "Projectile.h"
#include <iostream>
#include <bitset>

GameWorld::GameWorld()
{
	m_entitiesByType.resize(static_cast<int>(EntityType::COUNT));
}

void GameWorld::onDisconnect(Peer & peer, Server & server)
{
	if (peer.getEntity())
		peer.getEntity()->setAlive(false);
}

void GameWorld::prepare(Server & server)
{
	m_mapName = "grass";
	m_map.loadFromFile("map/grass.xml");
	for (const auto & p : server.getPlayers())
	{
		Human * h = static_cast<Human*>(createEntity(EntityType::HUMAN, p.get()));
		p->setEntity(h);
		Packer packer;
		packer.pack(Msg::SV_LOAD_GAME);
		p->send(packer, true);
	}


}

void GameWorld::onRequestGameInfo(Peer & peer, Server & server)
{
	//pack this player's id first
	Packer packer;
	packer.pack(Msg::SV_GAME_INFO);
	packer.pack(m_mapName);
	packer.pack<0, MAX_PLAYER_ID>(server.getPlayers().size());
	packer.pack<0, MAX_PLAYER_ID>(peer.getId());				//my player id
	packer.pack<0, MAX_ENTITY_ID>(peer.getEntity()->getId());	//my entity id
	packer.pack(peer.getEntity()->getType());					//my entity type

	for (const auto & p : server.getPlayers())
	{
		if (p->getId() != peer.getId())
		{
			packer.pack<0, MAX_PLAYER_ID>(p->getId());				//player id
			packer.pack<0, MAX_ENTITY_ID>(p->getEntity()->getId());	//entity id
		}
	}
	peer.send(packer, true);
}

void GameWorld::onInput(Peer & peer, Server & server, Unpacker & unpacker)
{
	int tick;
	uint32_t bits;
	unpacker.unpack<0, MAX_TICK>(tick);
	unpacker.unpack(bits);
	peer.onInput(bits, tick);
	if (m_tick % 2 == 0)
	{
		sf::Time timeLeft = sf::seconds(tick / TICKS_PER_SEC) - m_clock.getElapsedTime();

		Packer packer;
		packer.pack(Msg::SV_INPUT_TIMING);
		packer.pack<0, MAX_TICK>(tick);
		packer.pack(timeLeft.asMilliseconds());
		peer.send(packer, false);
	}

}

void GameWorld::start()
{
	Logger::getInstance().info("The game has started.");
	m_clock.restart();
}

void GameWorld::update(Server & server)
{
	static sf::Time accumulator, prevTime;
	sf::Time current = m_clock.getElapsedTime();
	sf::Time dt = current - prevTime;
	prevTime = current;
	accumulator += dt;

	while (accumulator >= sf::seconds(1.f / TICKS_PER_SEC))
	{
		m_tick++;
		accumulator -= sf::seconds(1.f / TICKS_PER_SEC);
	
		for (auto & v : m_entitiesByType)
			for (auto & e : v)
				e->tick(sf::seconds(1.f / TICKS_PER_SEC).asSeconds(), *this);

		auto isDead = [](std::unique_ptr<Entity> & e) {return !e->isAlive(); };
		for (auto & v : m_entitiesByType)
			v.erase(std::remove_if(v.begin(), v.end(), isDead), v.end());

		if (m_tick % 2 == 0)
			snap(server);
	}
}

int GameWorld::getCurrentTick()
{
	return m_tick;
}

const Map & GameWorld::getMap()
{
	return  m_map;
}

void GameWorld::reset()
{
	m_reset = true;
}

void GameWorld::snap(Server & server)
{
	Packer packer;
	packer.pack(Msg::SV_SNAPSHOT);
	packer.pack<0, MAX_TICK>(m_tick);
	Snapshot * snapshot = new Snapshot;

	for (const auto & v : m_entitiesByType)
		for (const auto & e : v)
			e->snap(*snapshot);
	snapshot->write(packer);

	for (auto & p : server.getPlayers())
		p->send(packer, false);
}

Entity * GameWorld::createEntity(EntityType type, Peer * p)
{
	Entity * e;
	switch (type)
	{
	case EntityType::HUMAN:
		e = new Human(m_nextEntityId++, p);
		break;
	case EntityType::PROJECTILE:
		e = new Projectile(m_nextEntityId++);
		break;
	default:
		return nullptr;
	}

	m_entitiesByType[static_cast<int>(type)].emplace_back(e);
	return e;
}

Entity * GameWorld::getEntity(int id, EntityType type)
{
	for (auto & e : m_entitiesByType[static_cast<int>(type)])
		if (e->getId() == id)
			return e.get();
	return nullptr;
}
