#pragma once

#include <memory>

#include "MetaComponent.h"

#include "../Resource/UploadBuffer.h"
#include "../Resource/Mesh.h"
#include "../Resource/Material.h"

#include "../Rendering/Transform.h"
#include "../Rendering/Renderer.h"

class StaticMeshComponent : public MetaComponent
{
private:
	std::unique_ptr<Renderer> m_renderer;

public:
	Transform m_transform;
	Mesh* m_mesh = NULL;
	Material* m_material = NULL;

	StaticMeshComponent();
	~StaticMeshComponent();

	void OnStart();
	void OnTick(double delta_time);
	void OnRender();
	void OnDestroy();
};



