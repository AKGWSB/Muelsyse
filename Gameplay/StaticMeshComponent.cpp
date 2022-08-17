#include "StaticMeshComponent.h"

StaticMeshComponent::StaticMeshComponent()
{
	m_cbPreObject = std::make_unique<UploadBuffer>(256);
}

StaticMeshComponent::~StaticMeshComponent()
{

}

void StaticMeshComponent::OnStart()
{

}

void StaticMeshComponent::OnTick(double delta_time)
{

}

void StaticMeshComponent::OnRender(ID3D12GraphicsCommandList* cmdList)
{
	if (m_material)
	{
		m_material->SetCbuffer("cbPreObject", m_cbPreObject.get());
		m_material->SetMatrix("cbPreObject", "modelMatrix", m_transform.GetTransformMatrix());
		m_material->Activate(cmdList);
	}

	if (m_mesh)
	{
		m_mesh->Draw(cmdList);
	}
}

void StaticMeshComponent::OnDestroy()
{

}
