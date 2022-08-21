#include "StaticMeshComponent.h"

#include "../Rendering/RenderQueue.h"
#include "../Library/DirectXTK/SimpleMath.h"

using namespace DirectX::SimpleMath;

StaticMeshComponent::StaticMeshComponent()
{
	m_renderer = std::make_unique<Renderer>();
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

void StaticMeshComponent::OnRender()
{
	// apply father's transform
	const Matrix& fatherTrans = GetOwner()->transform.GetTransformMatrix();
	const Matrix& myTrans = m_transform.GetTransformMatrix();
	Matrix finalTrans = fatherTrans * myTrans;

	// update renderer info
	m_renderer->transMat = finalTrans;
	m_renderer->material = m_material;
	m_renderer->mesh = m_mesh;

	// submit to global queue
	RenderQueue* rq = RenderQueue::GetInstance();
	rq->Submit(m_renderer.get());
}

void StaticMeshComponent::OnDestroy()
{

}
