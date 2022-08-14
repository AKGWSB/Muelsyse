#include "RenderTexture.h"
#include "../Core/GraphicContex.h"

RenderTexture::RenderTexture(int w, int h, DXGI_FORMAT fmt) :
	Texture2D(w, h, fmt, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
{
	GraphicContex* contex = GraphicContex::GetInstance();
	ID3D12Device* device = contex->GetDevice();
	ID3D12GraphicsCommandList* cmdList = contex->GetCommandList();
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

	// Describe and create a SRV for the texture.
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = fmt;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(m_buffer.Get(), &srvDesc, m_srvDescriptor.cpuHandle);

	// ba
	auto trans = CD3DX12_RESOURCE_BARRIER::Transition(m_buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	cmdList->ResourceBarrier(1, &trans);

	// debug
	//m_buffer->SetName(L"RT_DEBUG");
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
