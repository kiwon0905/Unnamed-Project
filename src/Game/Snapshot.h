#pragma once

#include "Game/NetObject.h"
#include "Core/Packer.h"
#include <unordered_map>
#include <memory>
#include <vector>

class Snapshot
{
public:
	enum
	{
		MAX_ITEM_SIZE = 1023
	};

	struct Key
	{
		int id;
		NetObject::Type type;
		bool operator==(const Key & key) const;
	};
	struct KeyHasher
	{
		std::size_t operator()(const Key & key) const;
	};

	const void * getEntity(NetObject::Type type, int id) const;

	void * addEntity(NetObject::Type type, int id);
	void * addTransient(NetObject::Type type);

	void read(Unpacker & unpacker);
	void write(Packer & packer);
	void readRelativeTo(Unpacker & unpacker, const Snapshot & s);
	void writeRelativeTo(Packer & packer, const Snapshot & s);

	const std::unordered_map<Key, std::unique_ptr<NetObject>, KeyHasher> & getEntities() const { return m_entities; }
	const std::vector<std::unique_ptr<NetObject>> & getTransients() const { return m_transients; }

	std::size_t getSize()
	{
		std::size_t size = 0;
		for (auto & p : m_entities)
			size += p.second->data.size();
		for (auto & o : m_transients)
			size += o->data.size();
		return size;
	}
private:
	std::unordered_map<Key, std::unique_ptr<NetObject>, KeyHasher> m_entities;
	std::vector<std::unique_ptr<NetObject>> m_transients;
};

struct TickedSnapshot
{
	TickedSnapshot(int tick, Snapshot * snapshot);
	int tick = -1;
	std::unique_ptr<Snapshot> snapshot;
};

class SnapshotContainer
{
public:
	Snapshot * get(int tick);
	Snapshot * getLast();
	void add(Snapshot * snapshot, int tick);
	void removeUntil(int tick);
	void clear();
	std::pair<TickedSnapshot*, TickedSnapshot*> find(float tick);
private:
	std::vector<TickedSnapshot> m_snapshots;
};