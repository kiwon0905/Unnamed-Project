#pragma once
#include "Entity.h"

class Projectile : public Entity
{
public:
	Projectile(int id);
	void rollback(const NetObject & e);
	void tick(float dt, const NetInput & input, Map & map);
	sf::Vector2f getCameraPosition(const Snapshot * from, const Snapshot * to, float predictedT, float t) const;
	void render(sf::RenderTarget & target, Client & client, PlayingScreen & ps, const Snapshot * from, const Snapshot * to, float predictedT, float t);
};