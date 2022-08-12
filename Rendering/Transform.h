#pragma once

#include "../Library/DirectXTK/SimpleMath.h"

using namespace DirectX::SimpleMath;

class Transform
{
public:
	Vector3 translate;
	Vector3 rotation;
	Vector3 scale = Vector3(1, 1, 1);

	Transform();
	~Transform();

	void Apply(Transform& t);
	void Apply(Vector3& t);

	Matrix GetTransformMatrix();
};

