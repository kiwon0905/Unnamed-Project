#include "PredictedEntity.h"

PredictedEntity::PredictedEntity(int id, Client & client, PlayingScreen & screen):
	Entity(id, EntityType::CHARACTER, client, screen)
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
