#pragma once

#include <memory>

#include "MetaComponent.h"

#include "../Rendering/Transform.h"
#include "../Resource/UploadBuffer.h"
#include "../Resource/Mesh.h"
#include "../Resource/Material.h"

class StaticMeshComponent : public MetaComponent
{
private:
	std::unique_ptr<UploadBuffer> m_cbPreObject;

public:
	Transform m_transform;
	Mesh* m_mesh = NULL;
	Material* m_material = NULL;

	StaticMeshComponent();
	~StaticMeshComponent();

	void OnStart();
	void OnTick(double delta_time);
	void OnRender(ID3D12GraphicsCommandList* cmdList);
	void OnDestroy();
};



