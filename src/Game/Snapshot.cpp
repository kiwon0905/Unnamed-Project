#include "Snapshot.h"
#include "GameConfig.h"
#include "Game/NetObject.h"

NetItem * Snapshot::getEntity(int id)
{
	if (m_entities.count(id))
		return m_entities[id].get();
	return nullptr;
}

NetItem * Snapshot::addEntity(NetItem::Type type, int id)
{
	if(m_entities.size() + m_events.size() >= MAX_SNAPSHOT_ITEM_SIZE)
		return nullptr;
	NetItem * entity = NetItem::create(type);
	m_entities[id].reset(entity);
	return entity;
}	

NetItem * Snapshot::addEvent(NetItem::Type type)
{
	if (m_entities.size() + m_events.size() >= MAX_SNAPSHOT_ITEM_SIZE)
		return nullptr;
	NetItem * event = NetItem::create(type);
	m_events.emplace_back(event);
	return event;
}

void Snapshot::read(Unpacker & unpacker)
{
	int numItems;
	unpacker.unpack<0, MAX_SNAPSHOT_ITEM_SIZE>(numItems);
	
	for (int i = 0; i < numItems; ++i)
	{
		NetItem::Type type;
		unpacker.unpack(type);

		NetItem * item = NetItem::create(type);
		
		if (type <= NetItem::HUMAN)
		{
			int id;
			unpacker.unpack<0, MAX_ENTITY_ID>(id);
			item->read(unpacker);
			m_entities[id].reset(item);
		}
		else
		{
			item->read(unpacker);
			m_events.emplace_back(item);
		}
	}
}

void Snapshot::write(Packer & packer)
{
	packer.pack<0, MAX_SNAPSHOT_ITEM_SIZE>(m_entities.size() + m_events.size());
	
	for (auto & e : m_entities)
	{
		packer.pack(e.second->getType());
		packer.pack<0, MAX_ENTITY_ID>(e.first);
		e.second->write(packer);
	}
	for (auto & e : m_events)
	{
		packer.pack(e->getType());
		e->write(packer);
	}

}


TickedSnapshot::TickedSnapshot(int tick, Snapshot * snapshot) :
	tick(tick),
	snapshot(snapshot)
{
}


Snapshot * SnapshotContainer::get(int tick)
{
	for (auto & s : m_snapshots)
		if (s.tick == tick)
			return s.snapshot.get();
	return  nullptr;
}

Snapshot * SnapshotContainer::getLast()
{
	if (m_snapshots.empty())
		return nullptr;
	return m_snapshots.back().snapshot.get();
}

void SnapshotContainer::add(Snapshot * snapshot, int tick)
{
	m_snapshots.emplace_back(tick, snapshot);
}

std::pair<TickedSnapshot*, TickedSnapshot*> SnapshotContainer::find(float tick)
{
	std::pair<TickedSnapshot*, TickedSnapshot*> pair;
	pair.first = nullptr;
	pair.second = nullptr;

	for (int i = m_snapshots.size() - 1; i >= 0; --i)
	{
		if (m_snapshots[i].tick <= tick)
		{
			pair.first = &m_snapshots[i];
			if (i + 1 < static_cast<int>(m_snapshots.size()))
				pair.second = &m_snapshots[i + 1];
			break;
		}
	}
	return pair;
}
