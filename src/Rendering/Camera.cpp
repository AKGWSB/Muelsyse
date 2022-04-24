#include "Camera.h"

Camera::Camera(float w, float h)
{
	aspect = w / h;
}

Camera::Camera()
{

}

Camera::~Camera()
{

}

void Camera::SetPosition(XMFLOAT3 pos)
{
	eye.x = pos.x;
	eye.y = pos.y;
	eye.z = pos.z;
}

void Camera::SetTarget(XMFLOAT3 target)
{
	at.x = target.x;
	at.y = target.y;
	at.z = target.z;
}

void Camera::SetAspect(float a)
{
	aspect = a;
}
void Camera::SetNear(float a)
{
	zNear = a;
}

void Camera::SetFar(float a)
{
	zFar = a;
}

XMFLOAT4X4 Camera::GetViewMatrix(bool useTranspose)
{
	XMVECTOR cPos = XMLoadFloat4(&eye);
	XMVECTOR cTarg = XMLoadFloat4(&at);
	XMVECTOR cUp = XMLoadFloat4(&up);

	XMMATRIX v = XMMatrixLookAtLH(cPos, cTarg, cUp);
	if (useTranspose)
	{
		v = XMMatrixTranspose(v);
	}
	XMStoreFloat4x4(&viewMatrix, v);

	return viewMatrix;
}

XMFLOAT4X4 Camera::GetProjectionMatrix(bool useTranspose)
{
	XMMATRIX p = XMMatrixPerspectiveFovLH(fov * (3.14f / 180.0f), aspect, zNear, zFar);

	if (useTranspose)
	{
		p = XMMatrixTranspose(p);
	}
	XMStoreFloat4x4(&projectionMatrix, p);

	return projectionMatrix;

}