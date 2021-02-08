#ifndef MOVINGOBJECT_H
#define MOVINGOBJECT_H

#include "GameObject.h"
#include "StaticObject.h"

class MovingObject : public GameObject
{
private:
	float m_moveSpeed;
	float m_rotationSpeed;
	Vector3 m_target;
	float m_facing;
	bool isMoving;
	//bool isRotating;
	//bool targetReached;
	bool targetSet;
	bool goalReached;
	float yRot;
	Vector3 displacementVector;
	std::vector<StaticObject*> m_waypoints;

public:
	MovingObject();
	MovingObject(Mesh* mesh, Shader* shader, float moveSpeed);
	MovingObject(Mesh* mesh, Shader* shader, Vector3 position, float moveSpeed);
	MovingObject(Mesh* mesh, Shader* shader, Texture* texture, float moveSpeed);
	MovingObject(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, float moveSpeed);
	MovingObject(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, float weight, std::vector<StaticObject*> waypoints);
	virtual ~MovingObject();

	float GetMoveSpeed() { return m_moveSpeed; }
	void SetMoveSpeed(float moveSpeed) { m_moveSpeed = moveSpeed; }

	void SetTarget();
	void Update(float timestep);
	bool GetGoalReachedStatus() { return goalReached; }
};

#endif