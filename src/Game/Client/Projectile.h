#pragma once
#include "Entity.h"

class Projectile : public Entity
{
public:
	Projectile(int id);
	void rollback(const Snapshot & s);
	void tick(float dt, unsigned input, Map & map);
	void preRender(const Snapshot * from, const Snapshot * to, float t);
	void render(sf::RenderTarget & target);
};