#pragma once
#include "Game/Client/Entity.h"

class SmokeTrailEmitter;

class Projectile : public Entity
{
public:
	Projectile(int id, Client & client, PlayingScreen & screen);
	~Projectile();

	const void * find(const Snapshot & s);

	sf::Vector2f getCameraPosition() const;
	void render();
private:
	SmokeTrailEmitter * m_emitter;
};