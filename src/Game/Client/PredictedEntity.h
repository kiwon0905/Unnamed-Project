#pragma once
#include "Entity.h"
#include "Game/Map.h"
#include "Game/NetObject.h"
#include "Game/NetInput.h"

class PredictedEntity : public Entity
{
public:
	PredictedEntity(int id, Client & client, PlayingScreen & screen);

	void setPrediction(bool predict);
	bool isPredicted() const;

	virtual bool rollback(Snapshot & snapshot) = 0;
	virtual void tick(float dt, const NetInput & input, Map & map) = 0;
private:
	bool m_predicted = false;
};