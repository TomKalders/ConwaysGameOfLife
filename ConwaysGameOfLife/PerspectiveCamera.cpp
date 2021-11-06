//#include "pch.h"
#include "PerspectiveCamera.h"
#include "gtx/transform.hpp"

PerspectiveCamera::PerspectiveCamera(const glm::fvec3& position, const glm::fvec3 forwardVector, float aspectRatio)
	: m_LookAt{ glm::mat4(1.f) }
	, m_ProjectionMatrix{ glm::mat4(1.f) }
	, m_AspectRatio{ aspectRatio }
	, m_Distance{1.f}
	, m_MovementSpeed{10.f}
	, m_RotationSpeed{30.f}
	, m_Near{0.1f}
	, m_Far{100.f}
{
	SetFieldOfView(45);
	SetPosition(position);
	SetForwardVector(forwardVector);
	CalculateVectors();
	CalculateProjectionMatrix();
}

//Getters
glm::fvec3 PerspectiveCamera::GetPosition() const
{
	//return glm::fvec3{ m_LookAt(0, 3), m_LookAt(1, 3), m_LookAt(2, 3) };
	return glm::fvec3{ m_LookAt[0].w, m_LookAt[1].w, m_LookAt[2].w };
}

glm::fvec3 PerspectiveCamera::GetForwardVector() const
{
	//return glm::fvec3{ m_LookAt(0, 2), m_LookAt(1, 2), m_LookAt(2, 2) };
	return glm::fvec3{ m_LookAt[0].z, m_LookAt[1].z, m_LookAt[2].z };
}

glm::fvec3 PerspectiveCamera::GetRightVector() const
{
	//return glm::fvec3{ m_LookAt(0, 0), m_LookAt(1, 0), m_LookAt(2, 0) };
	return glm::fvec3{ m_LookAt[0].x, m_LookAt[1].x, m_LookAt[2].x };
}

glm::fvec3 PerspectiveCamera::GetUpVector() const
{
	//return glm::fvec3{ m_LookAt(0, 1), m_LookAt(1, 1), m_LookAt(2, 1) };
	return glm::fvec3{ m_LookAt[0].y, m_LookAt[1].y, m_LookAt[2].y };
}

glm::mat4 PerspectiveCamera::GetLookAt() const
{
	return m_LookAt;
}

glm::mat4 PerspectiveCamera::GetViewMatrix() const
{
	return inverse(GetLookAt());
}

glm::mat4 PerspectiveCamera::GetProjectionMatrix() const
{
	return m_ProjectionMatrix;
}

float PerspectiveCamera::GetAspectRatio() const
{
	return m_AspectRatio;
}

float PerspectiveCamera::GetDistance() const
{
	return m_Distance;
}

float PerspectiveCamera::GetFieldOfView() const
{
	return m_Fov;
}

float PerspectiveCamera::GetMovementSpeed() const
{
	return m_MovementSpeed;
}

float PerspectiveCamera::GetRotationSpeed() const
{
	return m_RotationSpeed;
}

float PerspectiveCamera::GetFar() const
{
	return m_Far;
}

float PerspectiveCamera::GetNear() const
{
	return m_Near;
}

//Setters
void PerspectiveCamera::SetFieldOfView(float degrees)
{
	float angleInRadians{ glm::radians(degrees)/*degrees * float(E_TO_RADIANS)*/};
	float scaleFactor{ tanf(angleInRadians / 2.f) };
	m_Fov = atanf(scaleFactor / m_Distance);

	CalculateProjectionMatrix();
}

void PerspectiveCamera::SetDistance(float distance)
{
	m_Distance = distance;
}

void PerspectiveCamera::SetMovementSpeed(float speed)
{
	m_MovementSpeed = speed;
}

void PerspectiveCamera::SetRotationSpeed(float speed)
{
	m_RotationSpeed = speed;
}

void PerspectiveCamera::SetPosition(const glm::fvec3& position)
{
	
	m_LookAt[3].x = position.x;
	m_LookAt[3].y = position.y;
	m_LookAt[3].z = -position.z;
	m_LookAt[3].w = 1;
}

void PerspectiveCamera::SetFar(float farPlane)
{
	m_Far = farPlane;
	CalculateProjectionMatrix();
}

void PerspectiveCamera::SetNear(float nearPlane)
{
	m_Near = nearPlane;
	CalculateProjectionMatrix();
}

void PerspectiveCamera::Translate(const glm::fvec3& translation)
{
	//glm::mat4 translationMatrix = MakeTranslation(translation);
	glm::mat4 translationMatrix = glm::translate(translation);
	m_LookAt = translationMatrix * m_LookAt;
}

void PerspectiveCamera::Rotate(float angle, const glm::fvec3& axis, bool isDegrees)
{
	float rotation{ angle };
	if (isDegrees)
	{
		rotation = glm::radians(rotation);
	}
	glm::fvec3 translation = -glm::fvec3(GetPosition());

	Translate(translation);
	glm::mat4 rotationMatrix = glm::rotate(rotation, axis);
	m_LookAt = rotationMatrix * m_LookAt;
	Translate(-translation);

	CalculateVectors();
}

void PerspectiveCamera::RotateYaw(float angle, bool isDegrees)
{
	Rotate(angle, glm::fvec3{ 0, 1, 0 }, isDegrees);
}

void PerspectiveCamera::RotatePitch(float angle, bool isDegrees)
{
	Rotate(angle, GetRightVector(), isDegrees);
}

//Private Functions
void PerspectiveCamera::SetForwardVector(const glm::fvec3& forwardVector)
{
	//m_LookAt(0, 2) = forwardVector.x;
	//m_LookAt(1, 2) = forwardVector.y;
	//m_LookAt(2, 2) = forwardVector.z;
	m_LookAt[0].z = forwardVector.x;
	m_LookAt[1].z = forwardVector.y;
	m_LookAt[2].z = forwardVector.z;
}

void PerspectiveCamera::CalculateProjectionMatrix()
{
	//m_ProjectionMatrix(0, 0) = 1.f / (m_AspectRatio * m_Fov);
	//m_ProjectionMatrix(1, 1) = 1.f / m_Fov;
	//m_ProjectionMatrix(2, 2) = m_Far / (m_Far - m_Near);
	//m_ProjectionMatrix(3, 3) = 0;
	//m_ProjectionMatrix(3, 2) = 1;
	//m_ProjectionMatrix(2, 3) = -(m_Far * m_Near) / (m_Far - m_Near);
	m_ProjectionMatrix[0].x = 1.f / (m_AspectRatio * m_Fov);
	m_ProjectionMatrix[1].y = 1.f / m_Fov;
	m_ProjectionMatrix[2].z = m_Far / (m_Far - m_Near);
	m_ProjectionMatrix[3].w = 0;
	m_ProjectionMatrix[3].z = 1;
	m_ProjectionMatrix[2].w = -(m_Far * m_Near) / (m_Far - m_Near);
}

void PerspectiveCamera::CalculateVectors()
{
	glm::fvec3 forward = GetForwardVector();

	glm::fvec3 right = glm::normalize(glm::cross(glm::fvec3{ 0, 1, 0 }, forward));
	glm::fvec3 up = glm::normalize(glm::cross(forward, right));

	//m_LookAt(0, 0) = right.x;
	//m_LookAt(1, 0) = right.y;
	//m_LookAt(2, 0) = right.z;

	//m_LookAt(0, 1) = up.x;
	//m_LookAt(1, 1) = up.y;
	//m_LookAt(2, 1) = up.z;

	m_LookAt[0].x = right.x;
	m_LookAt[1].x = right.y;
	m_LookAt[2].x = right.z;

	m_LookAt[0].y = up.x;
	m_LookAt[1].y = up.y;
	m_LookAt[2].y = up.z;
}
