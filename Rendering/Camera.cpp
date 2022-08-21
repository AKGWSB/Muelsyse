#include "Camera.h"

#include <DirectXMath.h>

using namespace DirectX;

Camera::Camera()
{

}

Camera::~Camera()
{

}

Camera* Camera::GetMain()
{
	static Camera instance;
	return &instance;
}

inline Matrix LH_CreateLookAt(const Vector3& eye, const Vector3& target, const Vector3& up) noexcept
{
	using namespace DirectX;
	Matrix R;
	const XMVECTOR eyev = XMLoadFloat3(&eye);
	const XMVECTOR targetv = XMLoadFloat3(&target);
	const XMVECTOR upv = XMLoadFloat3(&up);
	XMStoreFloat4x4(&R, XMMatrixLookAtLH(eyev, targetv, upv));
	return R;
}

Matrix Camera::GetViewMatrix()
{
	Matrix trnasMat = trnasform.GetTransformMatrix();
	Vector3 forward = trnasMat.Forward();
	Vector3 eye = trnasform.translate;
	Vector3 target = eye + forward;
	Vector3 up = Vector3(0, 1, 0);	// Nana7mi

	return Matrix::CreateLookAt(eye, target, up).Transpose();
}

inline Matrix LH_CreatePerspectiveFieldOfView(float fov, float aspectRatio, float nearPlane, float farPlane) noexcept
{
	using namespace DirectX;
	Matrix R;
	XMStoreFloat4x4(&R, XMMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane));
	return R;
}

Matrix Camera::GetProjectionMatrix()
{
	if (useOrtho)
	{
		return Matrix::CreateOrthographic(orthoWidth, orthoHeight, nearPlane, farPlane).Transpose();
	}
	return LH_CreatePerspectiveFieldOfView(XMConvertToRadians(fovh), aspect, nearPlane, farPlane).Transpose();
}