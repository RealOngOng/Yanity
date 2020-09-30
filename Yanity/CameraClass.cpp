#include "stdafx.h"
#include "CameraClass.h"

CameraClass::CameraClass()
{
	m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
}
CameraClass::CameraClass(const CameraClass& other)
{

}
CameraClass::~CameraClass()
{

}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
}
void CameraClass::SetRoattion(float x, float y, float z)
{
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;
}

XMFLOAT3 CameraClass::GetPosition()
{
	return m_position;
}
XMFLOAT3 CameraClass::GetRoattion()
{
	return m_rotation;
}

void CameraClass::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	upVector = XMLoadFloat3(&up);

	position = m_position;
	positionVector = XMLoadFloat3(&position);

	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	lookAtVector = XMLoadFloat3(&lookAt);

	pitch = m_rotation.x * RADIAN;
	yaw = m_rotation.y * RADIAN;
	roll = m_rotation.z * RADIAN;

	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}