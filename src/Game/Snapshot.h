#pragma once

#include "Game/NetObject.h"
#include "Core/Packer.h"
#include <unordered_map>
#include <memory>
#include <vector>


class Snapshot
{
public:
	NetItem * getEntity(int id);

	NetItem * addEntity(NetItem::Type type, int id);
	NetItem * addEvent(NetItem::Type type);

	void read(Unpacker & unpacker);
	void write(Packer & packer);

	const std::unordered_map<int, std::unique_ptr<NetItem>> & getEntities() const { return m_entities; }
	const std::vector<std::unique_ptr<NetItem>> & getEvents() const { return m_events; }
private:
	std::unordered_map<int, std::unique_ptr<NetItem>> m_entities;
	std::vector<std::unique_ptr<NetItem>> m_events;
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
	void add(Snapshot * snapshot, int tick);

	std::pair<TickedSnapshot*, TickedSnapshot*> find(float tick);
private:
	std::vector<TickedSnapshot> m_snapshots;
};