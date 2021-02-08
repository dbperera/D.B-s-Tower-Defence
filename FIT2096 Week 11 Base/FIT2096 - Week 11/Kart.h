#ifndef KART_H
#define KART_H

#include "PhysicsObject.h"
#include "InputController.h"
#include "ItemBox.h"
#include "AudioSystem.h"

class Kart : public PhysicsObject
{
protected:
	InputController* m_input;
	CBoundingBox m_boundingBox;
	AudioSystem* m_audio;
	AudioClip* m_engineSound;

	float m_moveSpeed;
	float m_turnSpeed;

	Vector3 m_previousPosition;

public:
	Kart(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, InputController* input, AudioSystem* audio);
	~Kart();

	virtual void Update(float timestep);

	void OnKartCollisionEnter(Kart* other);
	void OnKartCollisionStay(Kart* other);
	void OnKartCollisionExit(Kart* other);

	void OnItemBoxCollisionEnter(ItemBox* other);
	void OnItemBoxCollisionStay(ItemBox* other);
	void OnItemBoxCollisionExit(ItemBox* other);

	CBoundingBox GetBounds() { return m_boundingBox; }
};

#endif
