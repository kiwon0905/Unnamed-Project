#pragma once

#include "Game/NetObject.h"
#include "Core/Packer.h"
#include <unordered_map>
#include <memory>
#include <vector>


class Snapshot
{
public:
	const NetObject * getEntity(int id) const;

	NetObject * addEntity(NetObject::Type type, int id);
	NetObject * addEvent(NetObject::Type type);

	void read(Unpacker & unpacker);
	void write(Packer & packer);

	const std::unordered_map<int, std::unique_ptr<NetObject>> & getEntities() const { return m_entities; }
	const std::vector<std::unique_ptr<NetObject>> & getTransientEntities() const { return m_transientEntities; }
private:
	std::unordered_map<int, std::unique_ptr<NetObject>> m_entities;
	std::vector<std::unique_ptr<NetObject>> m_transientEntities;
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