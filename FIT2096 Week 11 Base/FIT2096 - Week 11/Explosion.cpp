#include "Explosion.h"

Explosion::Explosion(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, AudioSystem* audio) :
	GameObject(mesh, shader, texture, position)
{
	uniformScale = 0.0;
	SetUniformScale(uniformScale);
	m_boundingSphere = CBoundingSphere(position, uniformScale * 19.73708);
	m_audio = audio;
	m_explosion = m_audio->Play("Assets/Sounds/explosion.mp3", true);
	if (m_explosion) {
		m_explosion->SetLoopCount(0);
		m_explosion->SetIs3D(true);
		m_explosion->SetMinMaxDistance(10.0, 90.0);
		m_explosion->SetPaused(false);
	}
}

void Explosion::Update(float timestep)
{
	uniformScale += 0.01;
	SetUniformScale(uniformScale);
	m_boundingSphere.SetRadius(uniformScale * 19.73708);
}

void Explosion::OnAgentCollisionEnter()
{
}

void Explosion::OnAgentCollisionStay()
{
}

void Explosion::OnAgentCollisionExit()
{
}
