#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
public:
	Camera();
	Camera(float w, float h);
	~Camera();

	void SetPosition(XMFLOAT3 pos);
	void SetTarget(XMFLOAT3 target);
	void SetAspect(float a);
	void SetNear(float a);
	void SetFar(float a);

	XMMATRIX GetViewMatrix(bool useTranspose = true);
	XMMATRIX GetOrientViewMatrix(bool useTranspose = true);
	XMMATRIX GetProjectionMatrix(bool useTranspose = true);

	XMFLOAT3 GetPosition();

private:
	float aspect = 1.0f;
	float fov = 60.0f;
	float zNear = 0.1f;
	float zFar = 10000.0f;

	XMFLOAT4 eye = XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);
	XMFLOAT4 at = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	XMFLOAT4 up = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
};
