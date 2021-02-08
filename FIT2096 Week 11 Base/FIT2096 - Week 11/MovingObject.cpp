#include "MovingObject.h"
#include "MathsHelper.h"

MovingObject::MovingObject() : GameObject()
{
	m_moveSpeed = 1.0f;
}

MovingObject::MovingObject(Mesh* mesh, Shader* shader, float moveSpeed) :
	GameObject(mesh, shader)
{
	m_moveSpeed = moveSpeed;
}

MovingObject::MovingObject(Mesh* mesh, Shader* shader, Vector3 position, float moveSpeed) :
	GameObject(mesh, shader, position)
{
	m_moveSpeed = moveSpeed;
}

MovingObject::MovingObject(Mesh* mesh, Shader* shader, Texture* texture, float moveSpeed) :
	GameObject(mesh, shader, texture)
{
	m_moveSpeed = moveSpeed;
}

MovingObject::MovingObject(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, float moveSpeed) :
	GameObject(mesh, shader, texture, position)
{
	m_moveSpeed = moveSpeed;
}

MovingObject::MovingObject(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, float weight, std::vector<StaticObject*> waypoints) :
	GameObject(mesh, shader, texture, position)
{
	m_waypoints = waypoints;

	//in order to avoid passing in three variables denoting move speed, rotation speed, and scale,
	//a single float known as weight is passed in, where the move speed and rotation speed is inversely
	//proportional to the size of the object
	m_moveSpeed = (2 / pow(weight, 2));
	m_rotationSpeed = (2 / pow(weight, 2));
	SetUniformScale(weight);

	//These are booleans that check what state the object is in, be it currently in movement, setting a target, or within the goal area
	isMoving = false;
	targetSet = false;
	goalReached = false;

	//The location of the closest waypoint
	m_target = Vector3::Zero;

	//The direction that the object must face to face the target
	m_facing = 0;

	//The current direction that the object is facing
	yRot = 0;

	//The displacement between the object and the target
	displacementVector = Vector3::Zero;
}

MovingObject::~MovingObject() { }

void MovingObject::SetTarget() {
	//if it's not moving, the target has not been set, and it hasn't reached the goal yet
	if (isMoving == false && targetSet == false && goalReached == false) {

		//Closest waypoint is initialised to be very far away
		Vector3 closestWaypoint = Vector3(100.0f, 100.0f, 100.0f);
		for (int i = 0; i < m_waypoints.size(); i++) {

			//if the waypoint is to the right of the object and closer than the previously designated closest waypoint that is not the waypoint we are currently at, we set it as our new closest waypoint
			if (m_waypoints[i]->GetPosition().x > m_position.x && (m_waypoints[i]->GetPosition() - m_position).Length() < (closestWaypoint - m_position).Length() && (m_waypoints[i]->GetPosition() - m_position).Length() > 0.1) {
				closestWaypoint = m_waypoints[i]->GetPosition();
				m_target = closestWaypoint;
			}
		}

		//generate our displacement vector, the direction we must face, and normalise our displacement vector
		displacementVector = m_target - m_position;
		m_facing = atan2(-displacementVector.z, displacementVector.x);
		displacementVector.Normalize();

		//this means we will not need to set another target until we have reached another waypoint
		targetSet = true;
	}
}

void MovingObject::Update(float timestep)
{
	//if we are in a moving state, update our location
	if (isMoving == true) {
		m_position += displacementVector * m_moveSpeed * timestep;
	}

	//if we are not in a moving state, then we are rotation, so we update our rotation
	else if (isMoving == false) {

		//this makes sure we rotate in the correct direction
		if (m_facing - (m_rotY - (float)ToRadians(90.0f)) > 0) {
			m_rotY += m_rotationSpeed * timestep;
		}
		else if (m_facing - (m_rotY - (float)ToRadians(90.0f)) <= 0) {
			m_rotY -= m_rotationSpeed * timestep;
		}
	}

	//if we have reached our destination, stop moving
	if ((m_target - m_position).Length() < 0.1) {
		isMoving = false;
		targetSet = false;
	}

	//but if we have not started moving yet but are facing the right direction, we start moving
	else if (m_facing - (m_rotY - (float)ToRadians(90.0f)) < 0.05 && m_facing - (m_rotY - (float)ToRadians(90.0f)) > -0.05) {
		isMoving = true;
	}

	//if we are within the goal area and have reached the waypoint within the goal, we state that the goal has been reached
	if (m_position.x > 14.43269435 && m_position.x < 20.20577209 && m_position.y > -14.43269435 && m_position.y < 14.43269435 && isMoving == false) {
		goalReached = true;
	}
}