#ifndef SHOCKWAVE_H
#define SHOCKWAVE_H

#include "GameObject.h"
#include "AudioSystem.h"

class Shockwave : public GameObject
{
private:
	CBoundingBox m_boundingBox;
	Vector3 m_target;
	Vector3 displacementVector;
	float m_moveSpeed;
	bool collided;
	float meshMinX;
	float meshMinY;
	float meshMinZ;
	float meshMaxX;
	float meshMaxY;
	float meshMaxZ;
	AudioSystem* m_audio;
	AudioClip* m_electricity;


public:
	Shockwave(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, AudioSystem* audio);

	void Update(float timestep);

	CBoundingBox GetBounds() { return m_boundingBox; }
	void OnAgentCollisionEnter();
	void OnAgentCollisionStay();
	void OnAgentCollisionExit();
};

#endif