#include "Human.h"
#include "Game/Client/GameWorld.h"

Human::Human(unsigned id):
	Entity(id, EntityType::HUMAN)
{

}

void Human::update(float dt, GameWorld & world)
{
	auto & snapshots = world.getSnapshots();
	


	if (m_predicted)
	{
		unsigned input = world.getLastInput();
		int direction = 0;
		bool jump = false;

		if (input & Control::MOVE_LEFT)
		{
			direction--;
		}
		if (input & Control::MOVE_RIGHT)
		{
			direction++;
		}
		if (input & Control::JUMP)
		{
			jump = true;
		}

		float acceleration = 2000.f;
		float maxVelocity = 300.f;
		float friction = 1000.f;

		float v = std::abs(m_velocity.x);
		if (direction != 0)
		{
			if (direction == 1)
			{
				m_velocity.x = maxVelocity;
			}
			else if (direction == -1)
			{
				m_velocity.x = -maxVelocity;
			}
		}
		else
		{
			m_velocity.x *= 1 / 10.f;
		}
		m_position += m_velocity  * dt;

		PastMove pastMove;
		pastMove.seq = world.getInputSeq();
		pastMove.dv = m_velocity * dt;
		m_pastMoveBuffer.push_back(pastMove);

	}
	else if (!snapshots.empty())
	{
		if (snapshots.back().entityInfo.count(m_id))
		{
			const GameWorld::Snapshot::EntityInfo & info = snapshots.back().entityInfo.at(m_id);
			m_position.x = info.x;
			m_position.y = info.y;
		}
	}
}

void Human::render(Renderer & renderer)
{
	renderer.renderHuman(m_position.x, m_position.y);
}
