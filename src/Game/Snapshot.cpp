#include "Snapshot.h"
#include "GameConfig.h"
#include "Game/NetObject.h"

const void * Snapshot::getEntity(int id) const
{
	auto iter = m_entities.find(id);
	if(iter != m_entities.end())
		return iter->second.get()->data.data();
	return nullptr;

}

void * Snapshot::addEntity(NetObject::Type type, int id)
{
	if(m_entities.size() + m_transients.size() >= MAX_SNAPSHOT_ITEM_SIZE)
		return nullptr;
	NetObject * entity = NetObject::create(type);
	m_entities[id].reset(entity);
	return entity->data.data();
}	

void * Snapshot::addTransient(NetObject::Type type)
{
	if (m_entities.size() + m_transients.size() >= MAX_SNAPSHOT_ITEM_SIZE)
		return nullptr;
	NetObject * event = NetObject::create(type);
	m_transients.emplace_back(event);
	return event->data.data();
}

void Snapshot::read(Unpacker & unpacker)
{
	int numItems;
	unpacker.unpack<0, MAX_SNAPSHOT_ITEM_SIZE>(numItems);
	
	for (int i = 0; i < numItems; ++i)
	{
		NetObject::Type type;
		unpacker.unpack(type);

		NetObject * item = NetObject::create(type);
		
		if (type < NetObject::ENTITY_COUNT)
		{
			int id;
			unpacker.unpack<0, MAX_ENTITY_ID>(id);

			item->read(unpacker);
			m_entities[id].reset(item);
		}
		else
		{
			item->read(unpacker);
			m_transients.emplace_back(item);
		}
	}

}

void Snapshot::write(Packer & packer)
{
	packer.pack<0, MAX_SNAPSHOT_ITEM_SIZE>(m_entities.size() + m_transients.size());
	
	for (auto & e : m_entities)
	{
		packer.pack(e.second->getType());
		packer.pack<0, MAX_ENTITY_ID>(e.first);
		e.second->write(packer);
	}
	for (auto & e : m_transients)
	{
		packer.pack(e->getType());
		e->write(packer);
	}
}

//delta compression can be further optimized: group unchanged entities, rle only entities, not transients, etc... 

void Snapshot::readRelativeTo(Unpacker & unpacker, const Snapshot & s)
{
	int numItems;
	unpacker.unpack<0, MAX_SNAPSHOT_ITEM_SIZE>(numItems);
	
	for (int i = 0; i < numItems; ++i)
	{
		NetObject::Type type;
		unpacker.unpack(type);

		NetObject * item = NetObject::create(type);

		if (type < NetObject::ENTITY_COUNT)
		{
			int id;
			unpacker.unpack<0, MAX_ENTITY_ID>(id);

			//check if 
			auto iter = s.m_entities.find(id);
			if (iter != s.m_entities.end())
			{
				NetObject * o = iter->second.get();
				item->readRelative(unpacker, *o);
			}
			else
			{
				item->read(unpacker);

			}
			m_entities[id].reset(item);
		}
		else
		{
			item->read(unpacker);
			m_transients.emplace_back(item);
		}
	}

}

void Snapshot::writeRelativeTo(Packer & packer, const Snapshot & s)
{
	packer.pack<0, MAX_SNAPSHOT_ITEM_SIZE>(m_entities.size() + m_transients.size());
	
	for (auto & e : m_entities)
	{
		packer.pack(e.second->getType());
		packer.pack<0, MAX_ENTITY_ID>(e.first);
		auto iter = s.m_entities.find(e.first);
		if (iter != s.m_entities.end())
		{
			NetObject * o = iter->second.get();
			e.second->writeRelative(packer, *o);
		}
		else
		{
			e.second->write(packer);
		}
	}

	for (auto & e : m_transients)
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

void SnapshotContainer::removeUntil(int tick)
{
	auto pred = [tick](const TickedSnapshot & s)
	{
		return s.tick <= tick;
	};
	m_snapshots.erase(std::remove_if(m_snapshots.begin(), m_snapshots.end(), pred), m_snapshots.end());
}

void SnapshotContainer::clear()
{
	m_snapshots.clear();
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
