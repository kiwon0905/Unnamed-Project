#include "Game/Server/GameContext.h"

class Vanilla : public GameContext
{
public:
	Vanilla(Server * server);
	std::string getName();
	void loadRound();
	void onRoundStart();

	void tick(float dt);
	void snap(Snapshot & snapshot);


	bool checkRound(Team & team);

	void reset();
private:
	Aabb m_controlPoint;

	Team m_controllingTeam = Team::NONE;
	int m_controlProgressA = 0;
	int m_controlProgressB = 0;
	
	Team m_capturingTeam = Team::NONE;
	int m_captureProgressA = 0;
	int m_captureProgressB = 0;

	int m_captureProgressDecay;
};