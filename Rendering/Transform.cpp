#include "Transform.h"

Transform::Transform()
{
	position = XMFLOAT3(0, 0, 0);
	rotation = XMFLOAT3(0, 0, 0);
	scale    = XMFLOAT3(1, 1, 1);
}

Transform::~Transform()
{

}

XMMATRIX Transform::GetTransformMatrix()
{
	/*
	XMMATRIX rotx = XMMatrixRotationX(XMConvertToRadians(rotation.x));
	XMMATRIX roty = XMMatrixRotationY(XMConvertToRadians(rotation.y));
	XMMATRIX rotz = XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	XMMATRIX m_rotation = rotx * roty * rotz;
	*/

	XMFLOAT3 rpy = XMFLOAT3(
		XMConvertToRadians(rotation.x), 
		XMConvertToRadians(rotation.y), 
		XMConvertToRadians(rotation.z)
	);
	XMMATRIX m_rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&rpy));

	XMMATRIX m_translate = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX m_scale = XMMatrixScaling(scale.x, scale.y, scale.z);

	// 返回之前需要转置
	// 并且因为是右乘矩阵, 变换顺序从左到右
	return XMMatrixTranspose(m_rotation * m_scale * m_translate);
}

void Transform::ApplyTransform(const Transform& t)
{
	
}

XMFLOAT3 Transform::Trans(const XMFLOAT3& v)
{
	XMFLOAT4 v4 = XMFLOAT4(v.x, v.y, v.z, 1.0);
	XMVECTOR V = XMLoadFloat4(&v4);
	XMMATRIX m = XMMatrixTranspose(GetTransformMatrix());

	XMFLOAT3 res;
	XMStoreFloat3(&res, XMVector4Transform(V, m));
	return res;
}