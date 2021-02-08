#ifndef HELLBOMB_H
#define HELLBOMB_H

#include "GameObject.h"
#include "Explosion.h"
#include "AudioSystem.h"

class Hellbomb : public GameObject
{
private:
	CBoundingBox m_boundingBox;
	Vector3 displacementVector;
	float m_moveSpeed;
	bool explosionTriggered;

public:
	Hellbomb(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position);

	void Update(float timestep);
	
	void SetExplosion(bool explosion) { explosionTriggered = explosion; }
	void Explode(Mesh* mesh, Texture * texture, std::vector<Explosion*>* explosion, AudioSystem* audio);

	CBoundingBox GetBounds() { return m_boundingBox; }
	void OnAgentCollisionEnter();
	void OnAgentCollisionStay();
	void OnAgentCollisionExit();
};

#endif