#ifndef BULLET_H
#define BULLET_H

#include "GameObject.h"

class Bullet : public GameObject
{
private:
	CBoundingBox m_boundingBox;
	Vector3 m_target;
	Vector3 displacementVector;
	float m_moveSpeed;
	bool collided;

public:
	Bullet(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, float uniformScale, float moveSpeed);

	void Update(float timestep);

	CBoundingBox GetBounds() { return m_boundingBox; }
	void OnAgentCollisionEnter();
	void OnAgentCollisionStay();
	void OnAgentCollisionExit();

	void SetTarget(Vector3 closestAgent);
	bool getCollided();
	void registerCollision();
};

#endif