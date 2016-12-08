#include "Human.h"
#include "Game/GameCore.h"
#include "Game/Control.h"
#include "Game/Server/Player.h"
#include "Core/MathUtility.h"

Human::Human(unsigned id, Player * player):
	Entity(id, EntityType::HUMAN, player)
{
}

void Human::update(float dt, GameWorld & world)
{
	if (m_player)
	{
		m_velocity.x = 0.f;
		Input * input = m_player->peekInput();
		if (input)
		{
			unsigned bits = input->bits;
			int direction = 0;
			bool jump = false;

			if (bits & Control::MOVE_LEFT)
				direction--;

			if (bits & Control::MOVE_RIGHT)
				direction++;

			if (bits & Control::JUMP)
				jump = true;

			float acceleration = 2000.f;
			float maxVelocity = 300.f;
			float friction = 1000.f;


			if (direction == 1)
				m_velocity.x = maxVelocity;
			else if (direction == -1)
				m_velocity.x = -maxVelocity;
			m_player->popInput();
		}
		m_position += m_velocity * dt;
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
