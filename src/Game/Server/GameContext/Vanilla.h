#include "Game/Server/GameContext.h"

class Vanilla : public GameContext
{
public:
	Vanilla(Server * server);
	std::string getName();
	void onRoundStart();
	bool checkRound(Team & team);
private:

};