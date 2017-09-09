#include "Game/Client/Entity.h"

class Crate : public Entity
{
public:
	Crate(int id, Client & client, PlayingScreen & screen);
	void rollback(const NetObject & e);

	void tick(float dt, const NetInput & input, Map & map);
	sf::Vector2f getCameraPosition(const Snapshot * from, const Snapshot * to, float predictedT, float t) const;

	void render(const Snapshot * from, const Snapshot * to, float predictedT, float t);

private:


};