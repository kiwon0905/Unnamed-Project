#include "Snapshot.h"
#include "GameConfig.h"
#include "Game/NetObject.h"
#include "Core/Rle.h"

bool Snapshot::Key::operator==(const Key & key) const
{
	return id == key.id && type == key.type;
}

std::size_t Snapshot::KeyHasher::operator()(const Key & key) const
{
	return key.id + 31 * key.type;
}

const void * Snapshot::getEntity(NetObject::Type type, int id) const
{
	Key key;
	key.id = id;
	key.type = type;
	auto iter = m_entities.find(key);
	if(iter != m_entities.end())
		return iter->second.get()->data.data();
	return nullptr;

}

void * Snapshot::addEntity(NetObject::Type type, int id)
{
	if(m_entities.size() + m_transients.size() >= MAX_ITEM_SIZE)
		return nullptr;

	Key key;
	key.id = id;
	key.type = type;

	NetObject * entity = NetObject::create(type);
	m_entities[key].reset(entity);
	return entity->data.data();
}	

void * Snapshot::addTransient(NetObject::Type type)
{
	if (m_entities.size() + m_transients.size() >= MAX_ITEM_SIZE)
		return nullptr;
	NetObject * event = NetObject::create(type);
	m_transients.emplace_back(event);
	return event->data.data();
}

void Snapshot::read(Unpacker & unpacker)
{
	int numItems;
	unpacker.unpack(numItems);

	//read all the types
	std::vector<NetObject::Type> types;
	for (int i = 0; i < numItems; ++i)
	{
		NetObject::Type type;
		unpacker.unpack(type);
		types.push_back(type);
	}

	for (NetObject::Type type : types)
	{
		NetObject * item = NetObject::create(type);
		if (type < NetObject::ENTITY_COUNT)
		{
			int id;
			unpacker.unpack(id);

			Key key;
			key.id = id;
			key.type = type;

			item->read(unpacker);
			m_entities[key].reset(item);
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
	packer.pack(m_entities.size() + m_transients.size());
	for (auto & e : m_entities)
	{
		packer.pack(e.first.type);
	}
	for (auto & e : m_transients)
	{
		packer.pack(e->getType());
	}


	for (auto & e : m_entities)
	{
		packer.pack(e.first.id);
		e.second->write(packer);
	}
	for (auto & e : m_transients)
	{
		e->write(packer);
	}
}

//delta compression can be further optimized: group unchanged entities, rle only entities, not transients, etc... 
void Snapshot::readRelativeTo(Unpacker & unpacker, const Snapshot & s)
{
	int numItems;
	unpacker.unpack(numItems);

	std::vector<NetObject::Type> types;
	for (int i = 0; i < numItems; ++i)
	{
		NetObject::Type type;
		unpacker.unpack(type);
		types.push_back(type);
	}

	for (NetObject::Type type : types)
	{
		NetObject * item = NetObject::create(type);

		if (type < NetObject::ENTITY_COUNT)
		{
			int id;
			unpacker.unpack(id);

			Key key;
			key.id = id;
			key.type = type;

			//check if 
			auto iter = s.m_entities.find(key);
			if (iter != s.m_entities.end())
			{
				NetObject * o = iter->second.get();
				item->readRelative(unpacker, *o);
			}
			else
			{
				item->read(unpacker);

			}
			m_entities[key].reset(item);
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
	packer.pack(m_entities.size() + m_transients.size());


	for (auto & e : m_entities)
	{
		packer.pack(e.first.type);
	}
	for (auto & e : m_transients)
	{
		packer.pack(e->getType());
	}

	for (auto & e : m_entities)
	{
		packer.pack(e.first.id);
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
