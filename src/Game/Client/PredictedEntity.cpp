#include "PredictedEntity.h"

PredictedEntity::PredictedEntity(int id, EntityType type, Client & client, PlayingScreen & screen):
	Entity(id, type, client, screen)
{
}

void PredictedEntity::setPrediction(bool predict)
{
	m_predicted = predict;
}

bool PredictedEntity::isPredicted() const
{
	return m_predicted;
}
