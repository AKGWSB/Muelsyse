#pragma once

#include "../Rendering/Transform.h"
#include "../Resource/Mesh.h"
#include "../Resource/Material.h"
#include "../Resource/UploadBuffer.h"

class Actor
{
public:
	std::string name;

	Transform transform;
	Mesh* mesh;
	Material* material;
	std::unique_ptr<UploadBuffer> cbufferPreObject;

	Actor();
	~Actor();
	void Draw();
};
