#ifndef STATICOBJECT_H
#define STATICOBJECT_H

#include "GameObject.h"

// This really doesn't do much at all. Because GameObject is abstract we need something to represent
// objects which basically sit there doing nothing. You could ditch this class and just use GameObject
// directly if you strip out the abstract stuff. We kept it in as an excuse to write an abstract class.

class StaticObject : public GameObject
{
public:
	StaticObject() : GameObject() {}
	StaticObject(Mesh* mesh, Shader* shader) : GameObject(mesh, shader) {}
	StaticObject(Mesh* mesh, Shader* shader, Texture* texture) : GameObject(mesh, shader, texture) {}
	StaticObject(Mesh* mesh, Shader* shader, Vector3 position) : GameObject(mesh, shader, position) {}
	StaticObject(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position) : GameObject(mesh, shader, texture, position) {}

	StaticObject(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, float uniformScale) : GameObject(mesh, shader, texture, position) { SetUniformScale(uniformScale); }

	//the scales are separated out so that different shapes can be constructed, such as the differing area of the ground objects
	StaticObject(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, float xscale, float yscale, float zscale) : GameObject(mesh, shader, texture, position, xscale, yscale, zscale) {}

	void Update(float timestep)
	{
		// Update does nothing for a static object
		// We need to provide an implementation though otherwise we will become abstract
		// (not worth moving this out into a seperate cpp file)
	}
};

#endif