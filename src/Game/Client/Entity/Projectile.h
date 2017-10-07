#pragma once
#include "Game/Client/Entity.h"

class SmokeTrailEmitter;

class Projectile : public Entity
{
public:
	Projectile(int id, Client & client, PlayingScreen & screen);
	~Projectile();

	sf::Vector2f getCameraPosition(const Snapshot * from, const Snapshot * to, float predictedT, float t) const;
	void render(const Snapshot * from, const Snapshot * to, float predictedT, float t);
private:
	SmokeTrailEmitter * m_emitter;
};