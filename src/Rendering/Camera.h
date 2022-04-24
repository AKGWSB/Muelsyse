#pragma once

#include <initguid.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>

#include "../DX12/d3dx12.h"
#include "../DX12/DirectXHelper.h"

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

	XMFLOAT4X4 GetViewMatrix(bool useTranspose = true);
	XMFLOAT4X4 GetProjectionMatrix(bool useTranspose = true);

private:
	float aspect = 1.0f;
	float fov = 60.0f;
	float zNear = 0.1f;
	float zFar = 1000.0f;

	XMFLOAT4 eye = XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);
	XMFLOAT4 at = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	XMFLOAT4 up = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;
};
