#ifndef SCENELIGHTING_H
#define SCENELIGHTING_H

#include <d3d11.h>
#include "DirectXTK/SimpleMath.h"

using namespace DirectX::SimpleMath;

class SceneLighting
{
private:
	Vector3 m_lightDirection;
	Color m_lightColour;
	Color m_ambientLight;

	Color m_fogColour;
	float m_fogStart;
	float m_fogEnd;

public:
	SceneLighting();
	SceneLighting(Vector3 lightDir, Color lightColour, Color ambientLight);
	SceneLighting(Vector3 lightDir, Color lightColour, Color ambientLight, Color fogColour, float fogStart, float fogEnd);
	~SceneLighting();

	Vector3 GetLightDirection() { return -m_lightDirection; }
	Color GetLightColour() { return m_lightColour; }
	Color GetAmbientLight() { return m_ambientLight; }
	Color GetFogColour() { return m_fogColour; }
	float GetFogStart() { return m_fogStart; }
	float GetFogEnd() { return m_fogEnd; }

	void SetLightDirection(Vector3 direction) { m_lightDirection = direction; }
	void SetLightColour(Color colour) { m_lightColour = colour; }
	void SetAmbientLight(Color ambient) { m_ambientLight = ambient; }
	void SetFogColour(Color colour) { m_fogColour = colour; }
	void SetFogStart(float fogStart) { m_fogStart = fogStart; }
	void SetFogEnd(float fogEnd) { m_fogEnd = fogEnd; }
};

#endif