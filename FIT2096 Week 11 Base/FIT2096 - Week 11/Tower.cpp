#include "Tower.h"
#include "MathsHelper.h"

Tower::Tower(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, float uniformScale, float fireRate, float range, std::vector<Agent*>* agents, AudioSystem* audio) :
	GameObject(mesh, shader, texture, position)
{
	m_fireCounter = 0;
	m_fireRate = fireRate;
	m_range = range;
	SetUniformScale(uniformScale);
	m_agents = agents;
	m_audio = audio;
}

void Tower::Update(float timestep) {

}

void Tower::ShootBullet(float timestep, Mesh* mesh, std::vector<Bullet*>* bullets, float uniformScale, float moveSpeed)
{
	if (m_agents->size() != 0 && SetTarget().z < 14.4327f) {
		m_fireCounter += timestep;
		if (m_fireCounter >= m_fireRate) {
			Bullet* bullet = new Bullet(mesh, GetShader(), GetTexture(), GetPosition(), uniformScale, moveSpeed);
			bullet->SetTarget(SetTarget());
			bullets->push_back(bullet);
			m_gunshot = m_audio->Play("Assets/Sounds/gunshot.mp3", true);
			if (m_gunshot) {
				m_gunshot->SetLoopCount(0);
				m_gunshot->SetIs3D(true);
				m_gunshot->SetMinMaxDistance(10.0, 90.0);
				m_gunshot->SetPaused(false);
			}
			m_fireCounter = 0;
		}
	}
}

Vector3 Tower::SetTarget() {
	Vector3 closestAgent = Vector3(100.0f, 100.0f, 100.0f);
	for (int i = 0; i < m_agents->size(); i++) {
		GameObject* agent = (*m_agents)[i];
		if ((agent->GetPosition() - m_position).Length() < (closestAgent - m_position).Length() && (agent->GetPosition() - m_position).Length() < m_range) {
			closestAgent = agent->GetPosition();
		}
	}
	return closestAgent;
}