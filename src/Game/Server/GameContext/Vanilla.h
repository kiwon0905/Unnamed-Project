#include "Game/Server/GameContext.h"

class Normal : public GameContext
{
public:
	Normal(Server * server);
	std::string getName();
	void prepareRound();
	bool checkRound(Team & team);
private:

};