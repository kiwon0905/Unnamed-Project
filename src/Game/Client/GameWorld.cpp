#include "GameWorld.h"
#include "Core/Protocol.h"
#include "Game/GameCore.h"
#include "Core/Client/Client.h"
#include "Core/MathUtility.h"
#include "Core/Logger.h"

void GameWorld::onDisconnect()
{
	m_ready = false;
}

void GameWorld::update(float dt, Client & client)
{
	if (m_ready)
	{


	}
}

void GameWorld::render(Client & client)
{
	if (m_ready)
	{
		float tick = m_snapshots.back().tick / static_cast<float>(SERVER_TICK_RATE) + m_lastSnapshot.getElapsedTime().asSeconds() - m_delay;
		client.getRenderer().renderText("tick: " + std::to_string(tick), 200.f, 200.f);
	}
}

void GameWorld::load()
{
}

void GameWorld::onWorldInfo(Unpacker & unpacker, Client & client)
{
	unpacker.unpack<ENTITY_ID_MIN, ENTITY_ID_MAX>(m_playerEntityId);
	Logger::getInstance().debug("My id is" + std::to_string(m_playerEntityId));
	Packer packer;
	packer.pack(Msg::CL_LOAD_COMPLETE);
	client.getNetwork().send(packer, true);
	Logger::getInstance().info("Loading complete");
	m_loaded = true;
}

void GameWorld::onSnapshot(Unpacker & unpacker, Client & client)
{
	int tick;
	unpacker.unpack<TICK_MIN, TICK_MAX>(tick);
	
	if (!m_snapshots.empty() && m_snapshots.back().tick > tick)
		return;

	Snapshot s;
	s.tick = tick;
	m_snapshots.push_back(s);



	if (!m_ready)
	{
		float renderTime = m_snapshots.back().tick / static_cast<float>(SERVER_TICK_RATE) - m_delay;
		float firstTime = m_snapshots.front().tick / static_cast<float>(SERVER_TICK_RATE);
		if (firstTime < renderTime)
		{
			std::cout << "ready!";
			m_ready = true;
			Packer packer;
			packer.pack(Msg::CL_READY);
			client.getNetwork().send(packer, true);
		}
	}







	m_lastSnapshot.restart();
}

const std::deque<GameWorld::Snapshot> & GameWorld::getSnapshots()
{
	return m_snapshots;
}

const std::deque <GameWorld::Input> & GameWorld::getInputs()
{
	return m_inputs;
}

void GameWorld::addEntity(Entity * e)
{
	m_entities.emplace_back(e);
}
