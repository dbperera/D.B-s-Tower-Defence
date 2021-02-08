#include "CollisionManager.h"

CollisionManager::CollisionManager(std::vector<Agent*>* agents, std::vector<Bullet*>* bullets, std::vector<Shockwave*>* shockwave, std::vector<Hellbomb*>* hellbombs, std::vector <Explosion*>* explosions, PlayerBase* playerBase)
{
	m_agents = agents;
	m_bullets = bullets;
	m_shockwave = shockwave;
	m_hellbombs = hellbombs;
	m_explosions = explosions;
	m_playerBase = playerBase;

	// Clear our arrays to 0 (NULL)
	memset(m_currentCollisions, 0, sizeof(m_currentCollisions));
	memset(m_previousCollisions, 0, sizeof(m_previousCollisions));

	m_nextCurrentCollisionSlot = 0;
}

void CollisionManager::CheckCollisions()
{
	// Check agent to bullet collisions
	AgentToBullet();

	//Check player base to agent collisions
	BaseToAgent();

	//Check agent to shockwave collisions
	AgentToShockwave();

	//Check agent to hellbomb collisions
	AgentToHellbomb();

	//Check agent to explosion collisions
	AgentToExplosion();

	// Move all current collisions into previous
	memcpy(m_previousCollisions, m_currentCollisions, sizeof(m_currentCollisions));

	// Clear out current collisions
	memset(m_currentCollisions, 0, sizeof(m_currentCollisions));
	
	// Now current collisions is empty, we'll start adding from the start again
	m_nextCurrentCollisionSlot = 0;

}

bool CollisionManager::ArrayContainsCollision(GameObject* arrayToSearch[], GameObject* first, GameObject* second)
{
	// See if these two GameObjects appear one after the other in specified collisions array
	// Stop one before length so we don't overrun as we'll be checking two elements per iteration
	for (int i = 0; i < MAX_ALLOWED_COLLISIONS - 1; i += 2)
	{
		if ((arrayToSearch[i] == first && arrayToSearch[i + 1] == second) ||
			arrayToSearch[i] == second && arrayToSearch[i + 1] == first)
		{
			// Found them!
			return true;
		}
	}

	// These objects were not colliding last frame
	return false;
}

void CollisionManager::AddCollision(GameObject* first, GameObject* second)
{	
	// Add the two colliding objects to the current collisions array
	// We keep track of the next free slot so no searching is required
	m_currentCollisions[m_nextCurrentCollisionSlot] = first;
	m_currentCollisions[m_nextCurrentCollisionSlot + 1] = second;
	
	m_nextCurrentCollisionSlot += 2;
}

void CollisionManager::AgentToBullet()
{
	// We'll check each kart against every item box
	// Note this is not overly efficient, both in readability and runtime performance

	for (unsigned int i = 0; i < m_agents->size(); i++)
	{
		for (unsigned int j = 0; j < m_bullets->size(); j++)
		{
			// Don't need to store pointer to these objects again but favouring clarity
			// Can't index into these directly as they're a pointer to a vector. We need to dereference them first
			Agent* agent = (*m_agents)[i];
			Bullet* bullet = (*m_bullets)[j];

			CBoundingBox agentBounds = agent->GetBounds();
			CBoundingBox bulletBounds = bullet->GetBounds();

			// Are they colliding this frame?
			bool isColliding = CheckCollision(agentBounds, bulletBounds);

			// Were they colliding last frame?
			bool wasColliding = ArrayContainsCollision(m_previousCollisions, agent, bullet);

			if (isColliding)
			{
				// Register the collision
				AddCollision(agent, bullet);

				if (wasColliding)
				{
					// We are colliding this frame and we were also colliding last frame - that's a collision stay
					// Tell the item box a kart has collided with it (we could pass it the actual kart too if we like)
					bullet->OnAgentCollisionStay();
					agent->OnBulletCollisionStay();
				}
				else
				{
					// We are colliding this frame and we weren't last frame - that's a collision enter
					bullet->OnAgentCollisionEnter();
					agent->OnBulletCollisionEnter();
				}
			}
			else
			{
				if (wasColliding)
				{
					// We aren't colliding this frame but we were last frame - that's a collision exit
					bullet->OnAgentCollisionExit();
					agent->OnBulletCollisionExit();
				}
			}
		}
	}
}

void CollisionManager::BaseToAgent()
{
	// We'll check each kart against every item box
	// Note this is not overly efficient, both in readability and runtime performance

	for (unsigned int i = 0; i < m_agents->size(); i++)
	{
		// Don't need to store pointer to these objects again but favouring clarity
			// Can't index into these directly as they're a pointer to a vector. We need to dereference them first
		Agent* agent = (*m_agents)[i];

		CBoundingSphere agentHitBox = agent->GetHitBox();
		CBoundingBox baseBounds = m_playerBase->GetBounds();

		// Are they colliding this frame?
		bool isColliding = CheckCollision(agentHitBox, baseBounds);

		// Were they colliding last frame?
		bool wasColliding = ArrayContainsCollision(m_previousCollisions, agent, m_playerBase);

		if (isColliding)
		{
			// Register the collision
			AddCollision(agent, m_playerBase);

			if (wasColliding)
			{
				// We are colliding this frame and we were also colliding last frame - that's a collision stay
				// Tell the item box a kart has collided with it (we could pass it the actual kart too if we like)
				m_playerBase->OnAgentHitBoxCollisionStay();
				agent->OnPlayerBaseCollisionStay();
			}
			else
			{
				// We are colliding this frame and we weren't last frame - that's a collision enter
				m_playerBase->OnAgentHitBoxCollisionEnter();
				agent->OnPlayerBaseCollisionEnter();
			}
		}
		else
		{
			if (wasColliding)
			{
				// We aren't colliding this frame but we were last frame - that's a collision exit
				m_playerBase->OnAgentHitBoxCollisionExit();
				agent->OnPlayerBaseCollisionExit();
			}
		}
	}
}
void CollisionManager::AgentToShockwave()
{
	// We'll check each kart against every item box
	// Note this is not overly efficient, both in readability and runtime performance

	for (unsigned int i = 0; i < m_agents->size(); i++)
	{
		for (unsigned int j = 0; j < m_shockwave->size(); j++) {
			// Don't need to store pointer to these objects again but favouring clarity
			// Can't index into these directly as they're a pointer to a vector. We need to dereference them first
			Agent* agent = (*m_agents)[i];
			Shockwave* shockwave = (*m_shockwave)[j];

			CBoundingBox agentBounds = agent->GetBounds();
			CBoundingBox shockwaveBounds = shockwave->GetBounds();

			// Are they colliding this frame?
			bool isColliding = CheckCollision(agentBounds, shockwaveBounds);

			// Were they colliding last frame?
			bool wasColliding = ArrayContainsCollision(m_previousCollisions, agent, shockwave);

			if (isColliding)
			{
				// Register the collision
				AddCollision(agent, shockwave);

				if (wasColliding)
				{
					// We are colliding this frame and we were also colliding last frame - that's a collision stay
					// Tell the item box a kart has collided with it (we could pass it the actual kart too if we like)
					shockwave->OnAgentCollisionStay();
					agent->OnShockwaveCollisionStay();
				}
				else
				{
					// We are colliding this frame and we weren't last frame - that's a collision enter
					shockwave->OnAgentCollisionEnter();
					agent->OnShockwaveCollisionEnter();
				}
			}
			else
			{
				if (wasColliding)
				{
					// We aren't colliding this frame but we were last frame - that's a collision exit
					shockwave->OnAgentCollisionExit();
					agent->OnShockwaveCollisionExit();
				}
			}
		}
		
	}
}

void CollisionManager::AgentToHellbomb()
{
	// We'll check each kart against every item box
	// Note this is not overly efficient, both in readability and runtime performance

	for (unsigned int i = 0; i < m_agents->size(); i++)
	{
		for (unsigned int j = 0; j < m_hellbombs->size(); j++) {
			// Don't need to store pointer to these objects again but favouring clarity
			// Can't index into these directly as they're a pointer to a vector. We need to dereference them first
			Agent* agent = (*m_agents)[i];
			Hellbomb* hellbomb = (*m_hellbombs)[j];

			CBoundingBox agentBounds = agent->GetBounds();
			CBoundingBox hellbombBounds = hellbomb->GetBounds();

			// Are they colliding this frame?
			bool isColliding = CheckCollision(agentBounds, hellbombBounds);

			// Were they colliding last frame?
			bool wasColliding = ArrayContainsCollision(m_previousCollisions, agent, hellbomb);

			if (isColliding)
			{
				// Register the collision
				AddCollision(agent, hellbomb);

				if (wasColliding)
				{
					// We are colliding this frame and we were also colliding last frame - that's a collision stay
					// Tell the item box a kart has collided with it (we could pass it the actual kart too if we like)
					hellbomb->OnAgentCollisionStay();
					agent->OnHellbombCollisionStay();
				}
				else
				{
					// We are colliding this frame and we weren't last frame - that's a collision enter
					hellbomb->OnAgentCollisionEnter();
					agent->OnHellbombCollisionEnter();
				}
			}
			else
			{
				if (wasColliding)
				{
					// We aren't colliding this frame but we were last frame - that's a collision exit
					hellbomb->OnAgentCollisionExit();
					agent->OnHellbombCollisionExit();
				}
			}
		}

	}
}

void CollisionManager::AgentToExplosion()
{
	// We'll check each kart against every item box
	// Note this is not overly efficient, both in readability and runtime performance

	for (unsigned int i = 0; i < m_agents->size(); i++)
	{
		for (unsigned int j = 0; j < m_explosions->size(); j++) {
			// Don't need to store pointer to these objects again but favouring clarity
			// Can't index into these directly as they're a pointer to a vector. We need to dereference them first
			Agent* agent = (*m_agents)[i];
			Explosion* explosion = (*m_explosions)[j];

			CBoundingBox agentBounds = agent->GetBounds();
			CBoundingSphere explosionBounds = explosion->GetBounds();

			// Are they colliding this frame?
			bool isColliding = CheckCollision(explosionBounds, agentBounds);

			// Were they colliding last frame?
			bool wasColliding = ArrayContainsCollision(m_previousCollisions, agent, explosion);

			if (isColliding)
			{
				// Register the collision
				AddCollision(agent, explosion);

				if (wasColliding)
				{
					// We are colliding this frame and we were also colliding last frame - that's a collision stay
					// Tell the item box a kart has collided with it (we could pass it the actual kart too if we like)
					explosion->OnAgentCollisionStay();
					agent->OnExplosionCollisionStay();
				}
				else
				{
					// We are colliding this frame and we weren't last frame - that's a collision enter
					explosion->OnAgentCollisionEnter();
					agent->OnExplosionCollisionEnter();
				}
			}
			else
			{
				if (wasColliding)
				{
					// We aren't colliding this frame but we were last frame - that's a collision exit
					explosion->OnAgentCollisionExit();
					agent->OnExplosionCollisionExit();
				}
			}
		}

	}
}
