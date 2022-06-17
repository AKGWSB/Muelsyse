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

XMMATRIX Camera::GetViewMatrix(bool useTranspose)
{
	XMVECTOR cPos = XMLoadFloat4(&eye);
	XMVECTOR cTarg = XMLoadFloat4(&at);
	XMVECTOR cUp = XMLoadFloat4(&up);

	XMMATRIX v = XMMatrixLookAtLH(cPos, cTarg, cUp);
	if (useTranspose)
	{
		v = XMMatrixTranspose(v);
	}
	return v;
}

XMMATRIX Camera::GetOrientViewMatrix(bool useTranspose)
{
	XMVECTOR cPos = XMLoadFloat4(&eye);
	XMVECTOR cTarg = XMLoadFloat4(&at);
	XMVECTOR cUp = XMLoadFloat4(&up);

	// move to (0, 0, 0)
	cTarg -= cPos;
	cPos = XMVectorSet(0, 0, 0, 0);
	
	XMMATRIX v = XMMatrixLookAtLH(cPos, cTarg, cUp);
	if (useTranspose)
	{
		v = XMMatrixTranspose(v);
	}
	return v;
}

XMMATRIX Camera::GetProjectionMatrix(bool useTranspose)
{
	XMMATRIX p = XMMatrixPerspectiveFovLH(XMConvertToRadians(fov), aspect, zNear, zFar);

	if (useTranspose)
	{
		p = XMMatrixTranspose(p);
	}
	return p;

}

XMFLOAT3 Camera::GetPosition()
{
	return XMFLOAT3(eye.x, eye.y, eye.z);
}