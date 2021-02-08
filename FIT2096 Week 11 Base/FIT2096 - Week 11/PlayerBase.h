#ifndef PLAYERBASE_H
#define PLAYERBASE_H

#include "GameObject.h"
#include "StaticObject.h"

class PlayerBase : public GameObject
{
private:
	CBoundingBox m_boundingBox;
	StaticObject healthBar;

public:
	PlayerBase(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, float uniformScale);

	void OnAgentHitBoxCollisionEnter();
	void OnAgentHitBoxCollisionStay();
	void OnAgentHitBoxCollisionExit();

	CBoundingBox GetBounds() { return m_boundingBox; };

	StaticObject GetHealthBar() { return healthBar; };

	void GenerateHealthBar(Mesh* mesh, Shader* shader, Texture* texture);
	bool getCollided();

	void Update(float timestep);
};

#endif
