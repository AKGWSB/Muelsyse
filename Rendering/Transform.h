#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Transform
{
public:
	XMFLOAT3 position;
	XMFLOAT3 rotation;	// XYZ Öá Ðý×ª½Ç¶È
	XMFLOAT3 scale;
	Transform();
	~Transform();

	XMMATRIX GetTransformMatrix();
	void ApplyTransform(const Transform& t);
	XMFLOAT3 Trans(const XMFLOAT3& v);
};
