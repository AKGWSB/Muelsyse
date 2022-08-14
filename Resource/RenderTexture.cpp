#include "RenderTexture.h"
#include "../Core/GraphicContex.h"

RenderTexture::RenderTexture(int w, int h, DXGI_FORMAT fmt) :
	Texture2D(w, h, fmt)
{
	GraphicContex* contex = GraphicContex::GetInstance();
	ID3D12Device* device = contex->GetDevice();
	DescriptorManager* descManager = DescriptorManager::GetInstance();

	// alloc rtv desc
	m_rtvDescriptor = descManager->AllocDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// create rtv
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = fmt;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Texture2D.PlaneSlice = 0;

	device->CreateRenderTargetView(m_buffer.Get(), &rtvDesc, m_rtvDescriptor.cpuHandle);
}

RenderTexture::~RenderTexture()
{
	DescriptorManager::GetInstance()->FreeDescriptor(m_rtvDescriptor);
}

CD3DX12_CPU_DESCRIPTOR_HANDLE RenderTexture::GetRtvCpuHandle()
{
	return m_rtvDescriptor.cpuHandle;
}

void RenderTexture::ChangeToRenderTargetState(ID3D12GraphicsCommandList* cmdList)
{
	auto br = CD3DX12_RESOURCE_BARRIER::Transition(m_buffer.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	cmdList->ResourceBarrier(1, &br);
}

void RenderTexture::ChangeToShaderRsourceState(ID3D12GraphicsCommandList* cmdList)
{
	auto br = CD3DX12_RESOURCE_BARRIER::Transition(m_buffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	cmdList->ResourceBarrier(1, &br);
}
