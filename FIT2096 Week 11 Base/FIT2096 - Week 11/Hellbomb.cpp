#include "Hellbomb.h"
#include "MathsHelper.h"

Hellbomb::Hellbomb(Mesh * mesh, Shader * shader, Texture * texture, Vector3 position) :
	GameObject(mesh, shader, texture, position)
{
	m_boundingBox = CBoundingBox(m_position + m_mesh->GetMin(), m_position + m_mesh->GetMax());
	SetUniformScale(20.0);
	displacementVector = Vector3(0.0, -1.0, 0.0);
	displacementVector.Normalize();
	m_moveSpeed = 0;
	SetXRotation((float)ToRadians(90.0));
	explosionTriggered = false;
}

void Hellbomb::Update(float timestep)
{
	m_moveSpeed += 10.0f;
	m_position += displacementVector * m_moveSpeed * timestep;
	if ((m_position + m_mesh->GetMin()).y <= 0) {
		SetUniformScale(GetXScale() - 0.1f);
	}
}

void Hellbomb::Explode(Mesh* mesh, Texture * texture, std::vector<Explosion*>* explosion, AudioSystem* audio)
{
	explosion->push_back(new Explosion(mesh, GetShader(), texture, GetPosition(), audio));
	explosionTriggered = true;
}

void Hellbomb::OnAgentCollisionEnter()
{
}

void Hellbomb::OnAgentCollisionStay()
{
}

void Hellbomb::OnAgentCollisionExit()
{
}
