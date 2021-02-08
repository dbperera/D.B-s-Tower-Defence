#include "Kart.h"

Kart::Kart(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, InputController* input, AudioSystem* audio) :
	PhysicsObject(mesh, shader, texture, position)
{
	m_input = input;
	m_audio = audio;

	m_moveSpeed = 0.08f;
	m_turnSpeed = 2.0f;

	// Try lowering the friction - it's good fun :)
	m_frictionAmount = 0.08f;

	m_boundingBox = CBoundingBox(m_position + m_mesh->GetMin(), m_position + m_mesh->GetMax());

	m_engineSound = m_audio->Play("Assets/Sounds/drumloop.wav", true);

	if (m_engineSound) {
		m_engineSound->SetLoopCount(-1);
		m_engineSound->SetIs3D(true);
		m_engineSound->SetMinMaxDistance(30.0, 200.0);
		m_engineSound->SetPaused(false);
	}
}

Kart::~Kart()
{

}

void Kart::Update(float timestep)
{
	// Rotation based on 'A' and 'D' keys
	if (m_input->GetKeyHold('A'))
	{
		m_rotY -= m_turnSpeed * timestep;
	}

	if (m_input->GetKeyHold('D'))
	{
		m_rotY += m_turnSpeed * timestep;
	}

	// This is the first time a moving object in our game can rotate and change where
	// it's looking. Suddenly moving along the world axes is not useful to us anymore.
	// We need to work out what direction is forward for this particular object. 

	// We'll start by declaring a forward vector in world space
	Vector3 worldForward = Vector3(0, 0, 1);

	// Next we'll wrap up our Y rotation in a matrix (remember matrices transform vectors)
	Matrix heading = Matrix::CreateRotationY(m_rotY);

	// Finally, we'll transform our world forward vector by the heading matrix which 
	// will essentially spin it from a world forward into a local forward which takes
	// the object's rotation into account.
	Vector3 localForward = Vector3::TransformNormal(worldForward, heading);

	if (m_input->GetKeyHold('W'))
	{
		ApplyForce(localForward * m_moveSpeed);
	}

	// Keep bounds up to date with position
	m_boundingBox.SetMin(m_position + m_mesh->GetMin());
	m_boundingBox.SetMax(m_position + m_mesh->GetMax());

	// TODO Set audio position and calculate our velocity
	if (m_engineSound) {
		Vector3 velocity = (m_position - m_previousPosition) / timestep;
		m_engineSound->Set3DAttributes(m_position, velocity);
	}

	m_previousPosition = m_position;

	PhysicsObject::Update(timestep);
}

void Kart::OnKartCollisionEnter(Kart* other)
{
	OutputDebugString("Kart-Kart Collision Enter\n");
}

void Kart::OnKartCollisionStay(Kart* other)
{
	OutputDebugString("Kart-Kart Collision Stay\n");
}

void Kart::OnKartCollisionExit(Kart* other)
{
	OutputDebugString("Kart-Kart Collision Exit\n");
}

void Kart::OnItemBoxCollisionEnter(ItemBox* other)
{
	OutputDebugString("Kart-ItemBox Collision Enter\n");

	// Bounce off box - how could you take our velocity into account?
	ApplyForce((m_position - other->GetPosition()) * 0.4f);
}

void Kart::OnItemBoxCollisionStay(ItemBox* other)
{
	OutputDebugString("Kart-ItemBox Collision Stay\n");
}

void Kart::OnItemBoxCollisionExit(ItemBox* other)
{
	OutputDebugString("Kart-ItemBox Collision Exit\n");
}
