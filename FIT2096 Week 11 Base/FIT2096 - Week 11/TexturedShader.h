/*	FIT2096 - Example Code
*	TexturedShader.h
*	Created by Elliott Wilson - 2015 - Monash University
*	This subclass of Shader extends the existing functionality so that we can texture our meshes
*	When we create a mesh we can now pass either a basic Shader or a TexturedShader to it
*	The methods here extend the Shader methods by calling them and then doing extra tasks afterwards.
*/

#ifndef TEXTUREDSHADER_H
#define TEXTUREDSHADER_H

#include "Shader.h"
#include "SceneLighting.h"

/*
The general process of adding a new buffer is as follows

- Declare the cbuffer in the vertex and/or pixel shader (depending on where its data is needed)
- Declare a matching struct in C++ that matches the cbuffer
- This struct will need padding added so no data crosses a 16 byte boundary
- The size of the struct as a whole should also be a multiple of 16 bytes

- Both the CPU and GPU now have a common understanding of the data that will flow between them

- Declare a new ID3D11Buffer in C++
- Create the buffer inside Shader::Initialise (or one of its children)
- Bind the buffer to the shader inside Shader::Begin (or one of its children)
- Send through the actual data inside Shader::SetConstants (or one of its children)
	- here we use map to upload the data in the GPU to system memory
	- then we play with it and set all the appropriate values
	- calling unmap copies it back to the GPU
- we can now access the data in our shaders using the varibles declared in our cbuffers
*/

class TexturedShader : public Shader
{
private:
	struct LightingBuffer
	{
		Color lightColour;
		// ----------------------- 16 byte boundary
		Color ambientColour;
		// ----------------------- 16 byte boundary
		Color fogColour;
		// ----------------------- 16 byte boundary
		Vector3 lightDirection;
		float fogStart;
		// ----------------------- 16 byte boundary
		float fogEnd;
		Vector3 padding;
		// ----------------------- 16 byte boundary
		// size = 16 * 5 = 80 bytes
	};

	SceneLighting* m_sceneLighting;
	ID3D11Buffer* m_lightingBuffer;

	ID3D11SamplerState* m_textureSampler;	//A sampler is an object that can be "sampled" across a range of values, we use them to represent textures in our shaders
											//A Texture is usually a 2D sampler where we can plug values in for the horizontal (u) position and the vertical (v) position
											//and it will give us a colour back.

	// Initialisation helpers
	bool InitSamplers(ID3D11Device* device);
	bool InitBuffers(ID3D11Device* device);

	bool SetLighting(ID3D11DeviceContext* context);

public:
	TexturedShader();
	TexturedShader(SceneLighting* sceneLighting);
	~TexturedShader();	//Destructor

	void Begin(ID3D11DeviceContext* context);
	bool Initialise(ID3D11Device* device, LPCWSTR vertexFilename, LPCWSTR pixelFilename);
	bool SetConstants(ID3D11DeviceContext* context, Matrix world, Matrix view, Matrix projection);
	bool SetTexture(ID3D11DeviceContext* context, ID3D11ShaderResourceView* textureView);
	void Release();
};

#endif