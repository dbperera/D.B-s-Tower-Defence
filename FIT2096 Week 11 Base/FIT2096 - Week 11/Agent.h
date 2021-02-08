#ifndef AGENT_H
#define AGENT_H

#include "Bullet.h"
#include "PlayerBase.h"
#include "GameObject.h"
#include "StaticObject.h"

class Agent : public GameObject
{
private:
	CBoundingBox m_boundingBox;
	CBoundingSphere m_hitBox;
	float m_moveSpeed;
	float m_rotationSpeed;
	Vector3 m_target;
	float m_facing;
	bool isMoving;
	bool targetSet;
	bool goalReached;
	float yRot;
	Vector3 displacementVector;
	std::vector<StaticObject*>* m_waypoints;
	PlayerBase* m_playerBase;
	StaticObject healthBar;
	float attackRate;
	float m_damage;
	bool m_InstaKill;
	float health;
	float armourScale;

public:
	Agent(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, float weight, float damage, bool InstaKill, float armourscale, std::vector<StaticObject*>* waypoints, PlayerBase* playerBase);
	~Agent();
	CBoundingBox GetBounds() { return m_boundingBox; }
	CBoundingSphere GetHitBox() { return m_hitBox; }

	void GenerateHealthBar(Mesh* mesh, Shader* shader, Texture* texture);
	StaticObject GetHealthBar() { return healthBar; };

	void OnBulletCollisionEnter();
	void OnBulletCollisionStay();
	void OnBulletCollisionExit();

	void OnPlayerBaseCollisionEnter();
	void OnPlayerBaseCollisionStay();
	void OnPlayerBaseCollisionExit();

	void OnShockwaveCollisionEnter();
	void OnShockwaveCollisionStay();
	void OnShockwaveCollisionExit();

	void OnHellbombCollisionEnter();
	void OnHellbombCollisionStay();
	void OnHellbombCollisionExit();

	void OnExplosionCollisionEnter();
	void OnExplosionCollisionStay();
	void OnExplosionCollisionExit();
	
	float GetMoveSpeed() { return m_moveSpeed; }
	void SetMoveSpeed(float moveSpeed) { m_moveSpeed = moveSpeed; }
	void SetDamage(float damage) { m_damage = damage; }
	void SetInstaKill(bool instaKill) { m_InstaKill = instaKill; }

	void SetTarget();
	void Update(float timestep);
	bool GetGoalReachedStatus() { return goalReached; }
	void SetArmourScale(float armourscale) { armourScale = armourscale; }

	void AttackBase(float timestep);

};

#endif