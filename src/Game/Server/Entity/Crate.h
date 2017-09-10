#include "Game/Server/Entity.h"

class Crate : public Entity
{
public:
	Crate(int id, GameContext * context, const sf::Vector2f & pos);

	void tick(float dt);
	void snap(Snapshot & snapshot) const;
	void takeDamage(int dmg);
private:
	int m_health;
};