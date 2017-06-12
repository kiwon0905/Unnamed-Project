#pragma once
#include "Entity.h"

class Projectile : public Entity
{
public:
	Projectile(int id);
	void rollback(const NetObject & e);
	void tick(float dt, const NetInput & input, Map & map);
	void preRender(const Snapshot * from, const Snapshot * to, float t);
	void render(sf::RenderTarget & target, Client & client);
};