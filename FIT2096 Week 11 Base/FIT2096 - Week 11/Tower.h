#ifndef TOWER_H
#define TOWER_H

#include "Agent.h"
#include "Bullet.h"
#include "GameObject.h"
#include "AudioSystem.h"

class Tower : public GameObject
{
private:
	float m_fireCounter;
	float m_fireRate;
	std::vector<Agent*>* m_agents;
	float m_range;
	AudioSystem* m_audio;
	AudioClip* m_gunshot;

public:
	Tower(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, float uniformScale, float fireRate, float range, std::vector<Agent*>* agents, AudioSystem* audio);

	void Update(float timestep);

	void ShootBullet(float timestep, Mesh* mesh, std::vector<Bullet*>* bullets, float uniformScale, float moveSpeed);

	void SetFireRate(float fireRate) { m_fireRate = fireRate; }
	void SetRange(float range) { m_range = range; }

	Vector3 SetTarget();
};

#endif