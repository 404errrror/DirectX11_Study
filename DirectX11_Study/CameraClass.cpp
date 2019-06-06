#include "stdafx.h"
#include "CameraClass.h"


CameraClass::CameraClass()
{
	m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
}


CameraClass::CameraClass(const CameraClass&)
{
}


CameraClass::~CameraClass()
{
}


void CameraClass::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
}


void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotation = XMFLOAT3(x, y, z);
}


XMFLOAT3 CameraClass::GetPosition()
{
	return m_position;
}


XMFLOAT3 CameraClass::GetRotation()
{
	return m_rotation;
}


void CameraClass::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	// ������ ����Ű�� ���͸� �����մϴ�.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// XMVECTOR ����ü�� �ε��մϴ�.
	upVector = DirectX::XMLoadFloat3(&up);

	// 3D���忡�� ī�޶��� ��ġ�� �����մϴ�.
	position = m_position;

	// XMVECTOR ����ü�� �ε��մϴ�
	positionVector = DirectX::XMLoadFloat3(&position);

	// �⺻������ ī�޶� �ٶ󺸴� ��ġ�� �����մϴ�.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// XMVECTOR ����ü�� �ε��Ѵ�.
	lookAtVector = DirectX::XMLoadFloat3(&lookAt);

	// yaw (y��), pitch (x��) �� roll (z��)�� ȸ������ ���� ������ �����մϴ�.
	pitch = m_rotation.x * 0.0174532925f;
	yaw = m_rotation.y * 0.0174532925f;
	roll =m_rotation.z * 0.0174532925f;

	// yaw, pitch, roll ���� ���� ȸ�� ����� ����ϴ�.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// lookAt �� up ���͸� ȸ�� ��ĸ� ����ؼ� �����Ͽ� �䰡 �������� �ùٸ��� ȸ���ǵ��� �մϴ�.
	// XMVector3TransformCoord : ���� * ���.
	lookAtVector = DirectX::XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = DirectX::XMVector3TransformCoord(upVector, rotationMatrix);

	// ȸ�� �� ī�޶� ��ġ�� ��� ��ġ�� ��ȯ�մϴ�.
	lookAtVector = DirectX::XMVectorAdd(positionVector, lookAtVector);

	// ���������� �� ���� ������Ʈ �� ���Ϳ��� �� ����� ����ϴ�
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
}


void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}