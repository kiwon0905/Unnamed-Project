#pragma once
#include "Game/Client/Entity.h"

class ParticleEmitter;

class Projectile : public Entity
{
public:
	Projectile(int id, Client & client, PlayingScreen & screen);
	~Projectile();
	void rollback(const NetObject & e);
	void tick(float dt, const NetInput & input, Map & map);
	sf::Vector2f getCameraPosition(const Snapshot * from, const Snapshot * to, float predictedT, float t) const;
	void render(const Snapshot * from, const Snapshot * to, float predictedT, float t);
private:
	ParticleEmitter * m_emitter;
};