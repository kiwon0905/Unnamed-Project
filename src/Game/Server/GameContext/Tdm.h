#include "Game/Server/GameContext.h"

class Tdm : public GameContext
{
public:
	Tdm(Server * server);
	std::string getName();
	void loadRound();
	void onRoundStart();

	void tick(float dt);
	void snap(Snapshot & snapshot);
	bool checkRound(Team & team);
	void reset();
	void onCharacterDeath(int killedPeer, int killerPeer, const std::unordered_map<int, int> & assisters);


private:
	int m_scoreA = 0;
	int m_scoreB = 0;
};