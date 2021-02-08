/*	FIT2096 - Example Code
*	TexturedShader.cpp
*	Created by Elliott Wilson - 2015 - Monash University
*	Implementation of TexturedShader.h
*/

#include "TexturedShader.h"

TexturedShader::TexturedShader() : Shader()
{
	m_sceneLighting = NULL;
	m_lightingBuffer = NULL;
	m_textureSampler = NULL;
}

TexturedShader::TexturedShader(SceneLighting* sceneLighting) : Shader()
{
	m_sceneLighting = sceneLighting;
	m_lightingBuffer = NULL;
	m_textureSampler = NULL;
}

TexturedShader::~TexturedShader() { }

bool TexturedShader::Initialise(ID3D11Device* device, LPCWSTR vertexFilename, LPCWSTR pixelFilename)
{
	if (!Shader::Initialise(device, vertexFilename, pixelFilename))		//We'll use the parent method to create most of the shader
		return false;

	if (!InitSamplers(device))
		return false;

	if (!InitBuffers(device))
		return false;
	
	return true;	
}

bool TexturedShader::InitSamplers(ID3D11Device* device)
{
	D3D11_SAMPLER_DESC textureSamplerDescription;	//When we create a sampler we need a Description struct to describe how we want to create the sampler

	//Now all we need to do is fill out our sampler description
	textureSamplerDescription.Filter = D3D11_FILTER_ANISOTROPIC;			//This is the Filtering method used for the texture... 
																			//...different values will give you different quality texture output
	textureSamplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		//This defines what happens when we sample outside of the range [0...1]
	textureSamplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;		//In our case we just want it to wrap around so that we always are sampling something
	textureSamplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	textureSamplerDescription.MipLODBias = 0.0f;							//The rest of these values are really just "Defaults"
	textureSamplerDescription.MaxAnisotropy = 8;							//If you want more info look up D3D11_SAMPLER_DESC on MSDN
	textureSamplerDescription.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	textureSamplerDescription.BorderColor[0] = 1;
	textureSamplerDescription.BorderColor[1] = 1;
	textureSamplerDescription.BorderColor[2] = 1;
	textureSamplerDescription.BorderColor[3] = 1;
	textureSamplerDescription.MinLOD = 0;
	textureSamplerDescription.MaxLOD = D3D11_FLOAT32_MAX;

	//and create the sampler!
	if (FAILED(device->CreateSamplerState(&textureSamplerDescription, &m_textureSampler)))
	{
		return false;
	}

	return true;
}

bool TexturedShader::InitBuffers(ID3D11Device* device)
{
	if (m_sceneLighting)
	{
		// Create lighting buffer
		D3D11_BUFFER_DESC lightBufferDescription;
		lightBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
		lightBufferDescription.ByteWidth = sizeof(LightingBuffer);
		lightBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		lightBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		lightBufferDescription.MiscFlags = 0;
		lightBufferDescription.StructureByteStride = 0;

		if (FAILED(device->CreateBuffer(&lightBufferDescription, NULL, &m_lightingBuffer)))
		{
			return false;
		}
	}

	return true;
}

void TexturedShader::Begin(ID3D11DeviceContext* context)
{
	// Let the parent class do most of the work
	Shader::Begin(context);	
	context->PSSetSamplers(0, 1, &m_textureSampler);

	if(m_lightingBuffer)
		context->PSSetConstantBuffers(0, 1, &m_lightingBuffer);
}

bool TexturedShader::SetConstants(ID3D11DeviceContext* context, Matrix world, Matrix view, Matrix projection)
{
	if (!Shader::SetConstants(context, world, view, projection))
		return false;

	if (!SetLighting(context))
		return false;

	return true;
}

bool TexturedShader::SetLighting(ID3D11DeviceContext* context)
{
	// Updating constant buffers each frame can be slow (we should really only re-send this info when it changes).
	// A "dirty" system like the camera uses could apply here.

	if (m_sceneLighting)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		LightingBuffer* inputData;

		//We call the Map method and pass in our Mapped Subresource struct, the method will fill it out for us
		if (FAILED(context->Map(m_lightingBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			return false;
		}

		//The pData pointer in the Mapped Subresource points to the memory within the buffer, so we cast it to a LightingBuffer
		inputData = (LightingBuffer*)mappedResource.pData;

		inputData->lightColour = m_sceneLighting->GetLightColour();
		inputData->ambientColour = m_sceneLighting->GetAmbientLight();
		inputData->fogColour = m_sceneLighting->GetFogColour();
		inputData->lightDirection = m_sceneLighting->GetLightDirection();
		inputData->fogStart = m_sceneLighting->GetFogStart();
		inputData->fogEnd = m_sceneLighting->GetFogEnd();
		inputData->padding = Vector3::Zero;

		// Unmapping the data sends it back to the GPU
		context->Unmap(m_lightingBuffer, 0);
	}

	return true;
}

bool TexturedShader::SetTexture(ID3D11DeviceContext* context, ID3D11ShaderResourceView* textureView)
{
	//This shader does have a texture so it does something in this method!
	context->PSSetShaderResources(0, 1, &textureView);
	return true;
}

void TexturedShader::Release()
{
	Shader::Release();
	if (m_textureSampler)
	{
		m_textureSampler->Release();
		m_textureSampler = NULL;
	}

	if (m_lightingBuffer)
	{
		m_lightingBuffer->Release();
		m_lightingBuffer = NULL;
	}
}