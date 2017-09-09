#include "Game/Server/Entity.h"

class Item : public Entity
{
public:
	Item(int id, GameContext * context);

	void tick(float dt);
	void snap(Snapshot & snapshot) const;

private:

};