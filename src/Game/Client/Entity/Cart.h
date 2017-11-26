#include "Game/Client/PredictedEntity.h"
#include "Game/GameCore.h"

class Cart : public Entity
{
public:
	Cart(int id, Client & client, PlayingScreen & screen);

	const void * find(const Snapshot & s);


	sf::Vector2f getCameraPosition(const Snapshot * from, const Snapshot * to, float predictedT, float t) const;
	void render(const Snapshot * from, const Snapshot * to, float predictedT, float t);

};