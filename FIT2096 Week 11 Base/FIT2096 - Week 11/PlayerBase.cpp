#include "PlayerBase.h"

PlayerBase::PlayerBase(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, float uniformScale) :
	GameObject(mesh, shader, texture, position)
{
	SetUniformScale(uniformScale);
	m_boundingBox = CBoundingBox(m_position + m_mesh->GetMin(), m_position + m_mesh->GetMax());
}

void PlayerBase::OnAgentHitBoxCollisionEnter()
{
	healthBar.SetXScale(healthBar.GetXScale() - 0.375);
}

void PlayerBase::OnAgentHitBoxCollisionStay()
{
}

void PlayerBase::OnAgentHitBoxCollisionExit()
{
}

void PlayerBase::GenerateHealthBar(Mesh* mesh, Shader* shader, Texture* texture)
{
	healthBar = StaticObject(mesh, shader, texture, m_position + Vector3(0, 5, 0), 15.0, 1.0, 1.0);
}

bool PlayerBase::getCollided()
{
	return false;
}

void PlayerBase::Update(float timestep)
{
}
