#pragma once

#include "Game/NetObject.h"
#include "Core/Packer.h"
#include <unordered_map>
#include <memory>
#include <vector>


class Snapshot
{
public:
	NetObject * getEntity(int id);

	NetObject * addEntity(NetObject::Type type, int id);
	NetObject * addEvent(NetObject::Type type);

	void read(Unpacker & unpacker);
	void write(Packer & packer);

	const std::unordered_map<int, std::unique_ptr<NetObject>> & getEntities() const { return m_entities; }
	const std::vector<std::unique_ptr<NetObject>> & getEvents() const { return m_events; }
private:
	std::unordered_map<int, std::unique_ptr<NetObject>> m_entities;
	std::vector<std::unique_ptr<NetObject>> m_events;
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

	std::pair<TickedSnapshot*, TickedSnapshot*> find(float tick);
private:
	std::vector<TickedSnapshot> m_snapshots;
};