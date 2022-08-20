#include "StaticMeshComponent.h"

#include "../Library/DirectXTK/SimpleMath.h"

using namespace DirectX::SimpleMath;

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
	const Matrix& fatherTrans = GetOwner()->transform.GetTransformMatrix();
	const Matrix& myTrans = m_transform.GetTransformMatrix();
	Matrix finalTrans = fatherTrans * myTrans;

	if (m_material)
	{
		m_material->SetCbuffer("cbPreObject", m_cbPreObject.get());
		m_material->SetMatrix("cbPreObject", "modelMatrix", finalTrans);
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
