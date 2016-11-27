#include "Human.h"
#include "Game/Client/GameWorld.h"
#include "Core/MathUtility.h"

Human::Human(unsigned id):
	Entity(id, EntityType::HUMAN)
{

}

void Human::update(float dt, GameWorld & world)
{
	auto & snapshots = world.getSnapshots();
	
	m_velocity.x = 0.f;
	if (m_predicted)
	{
		unsigned input = m_input;
		int direction = 0;
		bool jump = false;

		if (input & Control::MOVE_LEFT)
			direction--;
		if (input & Control::MOVE_RIGHT)
			direction++;
		if (input & Control::JUMP)
			jump = true;

		float acceleration = 2000.f;
		float maxVelocity = 300.f;
		float friction = 1000.f;


		if (direction == 1)
			m_velocity.x = maxVelocity;
		else if (direction == -1)
			m_velocity.x = -maxVelocity;

		m_position += m_velocity * dt;
	}
}

void Human::render(Renderer & renderer)
{
	renderer.renderHuman(m_position.x, m_position.y);
}
