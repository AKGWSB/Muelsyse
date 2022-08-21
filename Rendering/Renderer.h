#pragma once

#include "Transform.h"

#include "../Resource/Material.h"
#include "../Resource/Mesh.h"

#include "../Core/CommandListHandle.h"
#include "../Library/DirectXTK/SimpleMath.h"

using namespace DirectX::SimpleMath;

class Renderer
{
private:
	std::unique_ptr<UploadBuffer> m_cbPreObject;

public:
	friend class CommandListHandle;

	Matrix transMat;
	Mesh* mesh;
	Material* material;

	Renderer(const Matrix& t, Mesh* m, Material* mat);
	Renderer();
	~Renderer();

};


