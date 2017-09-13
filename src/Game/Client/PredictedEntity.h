#pragma once
#include "Entity.h"
#include "Game/Map.h"
#include "Game/NetObject.h"
#include "Game/NetInput.h"

class PredictedEntity : public Entity
{
public:
	PredictedEntity(int id, EntityType type, Client & client, PlayingScreen & screen);

	void setPrediction(bool predict);
	bool isPredicted() const;

	virtual void rollback(const NetObject & e) = 0;
	virtual void tick(float dt, const NetInput & input, Map & map) = 0;
private:
	bool m_predicted = false;
};