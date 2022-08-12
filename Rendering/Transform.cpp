#include "Transform.h"


Transform::Transform()
{

}

Transform::~Transform()
{

}

void Transform::Apply(Transform& t)
{

}

void Transform::Apply(Vector3& t)
{

}

Matrix Transform::GetTransformMatrix()
{
	const double PI = 3.1415926f;
	auto r = Matrix::CreateRotationX(rotation.x * PI / 180.0f) * Matrix::CreateRotationY(rotation.y * PI / 180.0f) * Matrix::CreateRotationZ(rotation.z * PI / 180.0f);
	auto t = Matrix::CreateTranslation(translate);
	auto s = Matrix::CreateScale(scale);

	auto m = (s * r * t).Transpose();

	return m;
}
