#pragma once

#include "Transform.h"

#include "../Library/DirectXTK/SimpleMath.h"

using namespace DirectX::SimpleMath;

class Camera
{
private:

public:
	bool useOrtho = false;
	float orthoWidth = 1.0f;
	float orthoHeight = 1.0f;

	float aspect = 1.0f;
	float fovh = 60.0f;
	float nearPlane = 0.05f;
	float farPlane = 1000.0f;
	Transform trnasform;

	Camera();
	~Camera();

	static Camera* GetMain();

	Matrix GetViewMatrix();
	Matrix GetProjectionMatrix();
};


