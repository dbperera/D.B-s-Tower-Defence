#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "GameObject.h"
#include "AudioSystem.h"

class Explosion : public GameObject
{
private:
	CBoundingSphere m_boundingSphere;
	float uniformScale;
	AudioSystem* m_audio;
	AudioClip* m_explosion;
	
public:
	Explosion(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, AudioSystem* audio);

	void Update(float timestep);

	CBoundingSphere GetBounds() { return m_boundingSphere; }
	void OnAgentCollisionEnter();
	void OnAgentCollisionStay();
	void OnAgentCollisionExit();
};

#endif