#include "Game/Server/GameContext.h"

class Infection : public GameContext
{
public:
	Infection(Server * server);
	std::string getName();
	void onRoundStart();
	bool checkRound(Team & team);
private:
};