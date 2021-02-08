#ifndef COLLISION_MANAGER_H
#define COLLISION_MANAGER_H

#include <vector>
#include "Collisions.h"
#include "Agent.h"
#include "Bullet.h"
#include "Shockwave.h"
#include "Hellbomb.h"
#include "Explosion.h"
#include "PlayerBase.h"

#define MAX_ALLOWED_COLLISIONS 2048

class CollisionManager
{
private:
	std::vector<Agent*>* m_agents;
	std::vector<Bullet*>* m_bullets;
	std::vector<Shockwave*>* m_shockwave;
	std::vector<Hellbomb*>* m_hellbombs;
	std::vector <Explosion*>* m_explosions;
	PlayerBase* m_playerBase;


	GameObject* m_currentCollisions[MAX_ALLOWED_COLLISIONS];

	// We need to know what objects were colliding last frame so we can determine if a collision has just begun or ended
	GameObject* m_previousCollisions[MAX_ALLOWED_COLLISIONS];

	int m_nextCurrentCollisionSlot;

	// Check if we already know about two objects colliding
	bool ArrayContainsCollision(GameObject* arrayToSearch[], GameObject* first, GameObject* second);

	// Register that a collision has occurred
	void AddCollision(GameObject* first, GameObject* second);

	// Collision check helpers
	void AgentToBullet();
	void BaseToAgent();
	void AgentToShockwave();
	void AgentToHellbomb();
	void AgentToExplosion();

public:
	CollisionManager(std::vector<Agent*>* agents, std::vector<Bullet*>* bullets, std::vector<Shockwave*>* shockwave, std::vector<Hellbomb*>* hellbombs,	std::vector <Explosion*>* explosions, PlayerBase* base);
	void CheckCollisions();

};

#endif