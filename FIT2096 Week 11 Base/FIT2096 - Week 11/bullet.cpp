#include "Bullet.h"
#include "MathsHelper.h"

Bullet::Bullet(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, float uniformScale, float moveSpeed) :
	GameObject(mesh, shader, texture, position)
{
	collided = false;
	SetUniformScale(uniformScale);
	m_boundingBox = CBoundingBox(m_position + m_mesh->GetMin(), m_position + m_mesh->GetMax());
	m_moveSpeed = moveSpeed;
	displacementVector = Vector3::Zero;
}

void Bullet::OnAgentCollisionEnter()
{
	registerCollision();
}

void Bullet::OnAgentCollisionStay()
{

}

void Bullet::OnAgentCollisionExit()
{

}

bool Bullet::getCollided() {
	return collided;
}

void Bullet::registerCollision() {
	collided = true;
}

void Bullet::SetTarget(Vector3 closestAgent) {
	displacementVector = closestAgent - m_position;
	displacementVector.Normalize();
}

void Bullet::Update(float timestep) {
	m_position += displacementVector * m_moveSpeed * timestep;
	m_boundingBox.SetMin(m_position + m_mesh->GetMin());
	m_boundingBox.SetMax(m_position + m_mesh->GetMax());
}