#include "Human.h"
#include "Game/GameCore.h"
#include "Core/MathUtility.h"

Human::Human(unsigned id, Player * player):
	Entity(id, EntityType::HUMAN, player)
{
}

void Human::update(float dt, GameWorld & world)
{
	if (!m_inputs.empty())
	{
		Input input = m_inputs.front();
		float acceleration = 2000.f;
		float maxVelocity = 300.f;
		float friction = 1000.f;
		
		float v = std::abs(m_velocity.x);
		if (input.direction != 0)
		{
			if (input.direction == 1)
			{
				m_velocity.x = maxVelocity;
			}
			else if(input.direction == -1)
			{
				m_velocity.x = -maxVelocity;
			}
		}
		else
		{
			m_velocity.x *= 1 / 10.f;
			/*if (m_velocity.x > 0)
			{
				m_velocity.x -= friction * dt;
				m_velocity.x = clamp(m_velocity.x, 0.f, maxVelocity);
			}
			else
			{
				m_velocity.x += friction *dt;
				m_velocity.x = clamp(m_velocity.x, 0.f, maxVelocity);
			}*/
		}
		
		std::cout << "velocity: " << m_velocity.x << "\n";

		m_position += m_velocity  * dt;


		m_inputs.pop();
	}
	else
	{
	}
}

void Human::snap(Packer & packer)
{
	//id,type,pos
	packer.pack<ENTITY_ID_MIN, ENTITY_ID_MAX>(m_id);
	packer.pack(m_type);
	packer.pack<2>(0.f, 5000.f, m_position.x);
	packer.pack<2>(0.f, 5000.f, m_position.y);
}
