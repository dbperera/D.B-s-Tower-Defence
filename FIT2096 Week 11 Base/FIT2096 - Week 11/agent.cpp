#include "Agent.h"
#include "MathsHelper.h"

Agent::Agent(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, float weight, float damage, bool InstaKill, float armourscale, std::vector<StaticObject*>* waypoints, PlayerBase* playerBase) :
	GameObject(mesh, shader, texture, position)
{
	m_waypoints = waypoints;
	m_playerBase = playerBase;
	attackRate = 2.0f;
	m_damage = damage;
	m_InstaKill = InstaKill;
	health = 3 * weight;
	armourScale = armourscale;

	//in order to avoid passing in three variables denoting move speed, rotation speed, and scale,
	//a single float known as weight is passed in, where the move speed and rotation speed is inversely
	//proportional to the size of the object
	m_moveSpeed = -weight + 2.5;
	m_rotationSpeed = -weight + 2.5;
	SetUniformScale(weight);

	m_boundingBox = CBoundingBox(m_position + m_mesh->GetMin(), m_position + m_mesh->GetMax());

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

Agent::~Agent() { }

void Agent::SetTarget() {
	//if it's not moving, the target has not been set, and it hasn't reached the goal yet
	if (isMoving == false && targetSet == false && goalReached == false) {

		//Closest waypoint is initialised to be very far away
		Vector3 closestWaypoint = Vector3(100.0f, 100.0f, 100.0f);
		for (int i = 0; i < m_waypoints->size(); i++) {
			StaticObject* waypoint = (*m_waypoints)[i];

			//if the waypoint is to the right of the object and closer than the previously designated closest waypoint that is not the waypoint we are currently at, we set it as our new closest waypoint
			if (waypoint->GetPosition().x > m_position.x && (waypoint->GetPosition() - m_position).Length() < (closestWaypoint - m_position).Length() && (waypoint->GetPosition() - m_position).Length() > 0.1 && (waypoint->GetPosition() - m_position).Length() < 17.0) {
				closestWaypoint = waypoint->GetPosition();
				m_target = closestWaypoint;
			}
		}
		
		if (closestWaypoint == Vector3(100.0f, 100.0f, 100.0f)) {
			m_target = m_playerBase->GetPosition();
		}

		//generate our displacement vector, the direction we must face, and normalise our displacement vector
		displacementVector = m_target - m_position;
		m_facing = atan2(-displacementVector.z, displacementVector.x);
		displacementVector.Normalize();

		//this means we will not need to set another target until we have reached another waypoint
		targetSet = true;
	}
}

void Agent::Update(float timestep)
{
	//if we are in a moving state, update our location
	if (isMoving == true) {
		m_position += displacementVector * m_moveSpeed * timestep;
		Vector3 position = Vector3(m_position.x, GetYScale(), m_position.z);
		healthBar.SetPosition(position);
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

	//if our m_target is not the player base
	if (m_target != m_playerBase->GetPosition()) {
		//if we have reached our destination, stop moving
		if ((m_target - m_position).Length() < 0.1) {
			isMoving = false;
			targetSet = false;
		}
		//but if we have not started moving yet but are facing the right direction, we start moving
		else if (m_facing - (m_rotY - (float)ToRadians(90.0f)) < 0.05 && m_facing - (m_rotY - (float)ToRadians(90.0f)) > -0.05) {
			isMoving = true;
		}
	}
	//if it is the base
	else {
		//if we have reached the base, stop moving
		if ((m_target - m_position).Length() < 3.5) {
			isMoving = false;
			targetSet = false;
			goalReached = true;
		}

		//but if we have not started moving yet but are facing the right direction, we start moving
		else if (m_facing - (m_rotY - (float)ToRadians(90.0f)) < 0.05 && m_facing - (m_rotY - (float)ToRadians(90.0f)) > -0.05) {
			isMoving = true;
		}
	}
	m_boundingBox.SetMin(m_position + m_mesh->GetMin());
	m_boundingBox.SetMax(m_position + m_mesh->GetMax());
}

void Agent::AttackBase(float timestep)
{
	if (attackRate > -0.001 && attackRate < 0.001) {
		m_hitBox = CBoundingSphere(m_position, 0.0f);
		attackRate += timestep;
	}
	else if (attackRate > 0.001 && attackRate < 1.999) {
		attackRate += timestep;
	}
	else if (attackRate > 1.999 && attackRate < 2.001) {
		Vector3 m_hitBoxOrigin = (m_target - m_position) / 10;
		m_hitBox = CBoundingSphere(m_position + m_hitBoxOrigin, 1.0f);
		attackRate = 0;
	}
}

void Agent::GenerateHealthBar(Mesh* mesh, Shader* shader, Texture* texture)
{
	Vector3 position = Vector3(m_position.x, GetYScale(), m_position.z);
	healthBar = StaticObject(mesh, shader, texture, position, health, 1.0, 1.0);
}

void Agent::OnBulletCollisionEnter()
{
	if (!m_InstaKill) {
		healthBar.SetXScale(healthBar.GetXScale() - m_damage * armourScale);
	}
	else {
		healthBar.SetXScale(0);
	}
}

void Agent::OnBulletCollisionStay()
{
}

void Agent::OnBulletCollisionExit()
{
}

void Agent::OnPlayerBaseCollisionEnter()
{
}

void Agent::OnPlayerBaseCollisionStay()
{
}

void Agent::OnPlayerBaseCollisionExit()
{
}

void Agent::OnShockwaveCollisionEnter()
{
	healthBar.SetXScale(healthBar.GetXScale() - 2.0f);
}

void Agent::OnShockwaveCollisionStay()
{
}

void Agent::OnShockwaveCollisionExit()
{
}

void Agent::OnHellbombCollisionEnter()
{
	healthBar.SetXScale(healthBar.GetXScale() - 2.0f);
}

void Agent::OnHellbombCollisionStay()
{
}

void Agent::OnHellbombCollisionExit()
{
}

void Agent::OnExplosionCollisionEnter()
{
	healthBar.SetXScale(healthBar.GetXScale() - 3.0f);
}

void Agent::OnExplosionCollisionStay()
{
}

void Agent::OnExplosionCollisionExit()
{
}

