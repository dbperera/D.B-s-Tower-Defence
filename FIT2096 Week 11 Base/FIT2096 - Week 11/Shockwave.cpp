#include "Shockwave.h"
#include "MathsHelper.h"

Shockwave::Shockwave(Mesh * mesh, Shader * shader, Texture * texture, Vector3 position, AudioSystem* audio) :
	GameObject(mesh, shader, texture, position)
{
	SetUniformScale(10.0f);
	SetXScale(10.0f);
	SetYRotation((float)ToRadians(90.0f));
	meshMinX = -2.0f;
	meshMinY = 0.0f;
	meshMinZ = -15.0f;
	meshMaxX = 2.0f;
	meshMaxY = 8.0f;
	meshMaxZ = 15.0f;
	m_boundingBox = CBoundingBox(m_position + Vector3(meshMinX, meshMinY, meshMinZ), m_position + Vector3(meshMaxX, meshMaxY, meshMaxZ));
	m_moveSpeed = 20.0f;
	displacementVector = Vector3(-1.0, 0.0, 0.0);
	displacementVector.Normalize();
	m_audio = audio;
	m_electricity = m_audio->Play("Assets/Sounds/electricity.mp3", true);
	if (m_electricity) {
		m_electricity->SetLoopCount(1);
		m_electricity->SetIs3D(true);
		m_electricity->SetMinMaxDistance(30.0, 200.0);
		m_electricity->SetPaused(false);
	}
}

void Shockwave::Update(float timestep)
{
	if (GetXScale() < 59.99f) {
		SetXScale(GetXScale() + 5.0f);
	}
	m_position += displacementVector * m_moveSpeed * timestep;
	m_boundingBox.SetMin(m_position + Vector3(meshMinX, meshMinY, meshMinZ));
	m_boundingBox.SetMax(m_position + Vector3(meshMaxX, meshMaxY, meshMaxZ));
}

void Shockwave::OnAgentCollisionEnter()
{
}

void Shockwave::OnAgentCollisionStay()
{
}

void Shockwave::OnAgentCollisionExit()
{
}