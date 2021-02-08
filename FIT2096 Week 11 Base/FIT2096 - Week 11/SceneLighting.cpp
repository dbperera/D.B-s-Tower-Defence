#include "SceneLighting.h"

SceneLighting::SceneLighting()
{
	m_lightDirection = Vector3(0.5, -0.5, 0.5);
	m_lightDirection.Normalize();
	m_lightColour = Color(1.0f, 1.0f, 1.0f, 1.0f);
	m_ambientLight = Color(0.2f, 0.2f, 0.2f, 1.0f);
	m_fogColour = Color(0.3f, 0.3f, 0.3f, 1.0f);
	m_fogStart = 10.0f;
	m_fogEnd = 500.0f;
}

SceneLighting::SceneLighting(Vector3 lightDir, Color lightColour, Color ambientLight)
{
	m_lightDirection = lightDir;
	m_lightDirection.Normalize();
	m_lightColour = lightColour;
	m_ambientLight = ambientLight;
	m_fogColour = Color(0.3f, 0.3f, 0.3f, 1.0f);
	m_fogStart = 10.0f;
	m_fogEnd = 500.0f;
}

SceneLighting::SceneLighting(Vector3 lightDir, Color lightColour, Color ambientLight, Color fogColour, float fogStart, float fogEnd)
{
	m_lightDirection = lightDir;
	m_lightDirection.Normalize();
	m_lightColour = lightColour;
	m_ambientLight = ambientLight;
	m_fogColour = fogColour;
	m_fogStart = fogStart;
	m_fogEnd = fogEnd;
}

SceneLighting::~SceneLighting()
{

}