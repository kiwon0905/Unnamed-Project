#include "Game/Server/GameContext.h"

class Infection : public GameContext
{
public:
	std::string getName();
	void startRound();
	void checkRound();
private:
};