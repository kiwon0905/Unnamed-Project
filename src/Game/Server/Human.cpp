#include "Human.h"
#include "Game/GameCore.h"

Human::Human(unsigned id, Player * player):
	Entity(id, EntityType::HUMAN, player)
{
}

void Human::update(float dt, GameWorld & world)
{
	if (!m_inputs.empty())
	{
		Input input = m_inputs.front();
		float acceleration = 100.f;
		float maxVelocity = 500.f;
		
		float v = std::abs(m_velocity.x);
		if (input.direction != 0)
		{
			v += acceleration * dt;
			if (input.direction == 1)
			{
				m_velocity.x = v;
			}
			else if(input.direction == -1)
			{
				m_velocity.x = -v;
			}

		}
		
		
		m_position += m_velocity  * dt;


	std::cout << "pos: " << m_position.x << "\n";
		m_inputs.pop();
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
