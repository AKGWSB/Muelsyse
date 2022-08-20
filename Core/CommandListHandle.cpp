#include "CommandListHandle.h"
#include "GraphicContex.h"

CommandListHandle::CommandListHandle(ID3D12GraphicsCommandList* cmdList)
{
	m_cmdList = cmdList;
}

CommandListHandle::~CommandListHandle()
{

}

// ---------------------------------------------------------------------- //

void CommandListHandle::SetRenderTarget(std::vector<RenderTexture*> renderTargets, DepthTexture* depthTex)
{
	D3D12_CPU_DESCRIPTOR_HANDLE hd;
	D3D12_CPU_DESCRIPTOR_HANDLE* pDsvHandle = NULL;
	if (depthTex)
	{
		hd = depthTex->GetDsvCpuHandle();
		pDsvHandle = &hd;
	}

	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles;
	for (auto& rt : renderTargets)
	{
		rtvHandles.push_back(rt->GetRtvCpuHandle());
	}
	m_cmdList->OMSetRenderTargets(rtvHandles.size(), rtvHandles.data(), FALSE, pDsvHandle);
}

void CommandListHandle::ClearRenderTarget(std::vector<RenderTexture*> renderTargets, Vector3 clearColor)
{
	float _clearColor[4] = { clearColor.x, clearColor.y, clearColor.z, 1.0f };

	for (auto& rt : renderTargets)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rt->GetRtvCpuHandle();
		m_cmdList->ClearRenderTargetView(rtvHandle, _clearColor, 0, nullptr);
	}
}

void CommandListHandle::ClearDepthBuffer(DepthTexture* depthTex)
{
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthTex->GetDsvCpuHandle();
	m_cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void CommandListHandle::SetViewPort(Vector4 rect)
{
	CD3DX12_VIEWPORT m_viewport(rect.x, rect.y, rect.z, rect.w);
	CD3DX12_RECT m_scissorRect(rect.x, rect.y, rect.z, rect.w);
	m_cmdList->RSSetViewports(1, &m_viewport);
	m_cmdList->RSSetScissorRects(1, &m_scissorRect);
}

void CommandListHandle::RenderActor(Actor* actor)
{
	actor->OnRender(m_cmdList);
}

