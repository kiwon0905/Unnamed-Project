#include "Game/Server/GameContext.h"

class Normal : public GameContext
{
public:
	std::string getName();
	void startRound();
	void checkRound();
private:

};