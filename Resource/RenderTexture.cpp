#include "RenderTexture.h"

#include "../Core/GraphicContex.h"
#include "../Core/helper.h"

RenderTexture::RenderTexture(int w, int h, DXGI_FORMAT fmt)
{
	GraphicContex* contex = GraphicContex::GetInstance();
	ID3D12Device* device = contex->GetDevice();
	DescriptorManager* descManager = DescriptorManager::GetInstance();

	width = w;
	height = h;
	m_format = fmt;

	// Describe and create a Texture2D.
	D3D12_RESOURCE_DESC textureDesc = {};
	textureDesc.MipLevels = 1;
	textureDesc.Format = m_format;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	textureDesc.DepthOrArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	auto hp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(device->CreateCommittedResource(
		&hp,
		D3D12_HEAP_FLAG_NONE,
		&textureDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&m_buffer)));

	// alloc rtv. srv desc
	m_rtvDescriptor = descManager->AllocDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_srvDescriptor = descManager->AllocDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//  Describe and create a rtv for the texture.
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
}

RenderTexture::~RenderTexture()
{
	DescriptorManager* descManager = DescriptorManager::GetInstance();

	descManager->FreeDescriptor(m_rtvDescriptor);
	descManager->FreeDescriptor(m_srvDescriptor);
}

CD3DX12_CPU_DESCRIPTOR_HANDLE RenderTexture::GetRtvCpuHandle()
{
	return m_rtvDescriptor.cpuHandle;
}

/*
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
*/

void RenderTexture::ChangeState(ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES curStat, D3D12_RESOURCE_STATES dstStat)
{
	auto br = CD3DX12_RESOURCE_BARRIER::Transition(m_buffer.Get(), curStat, dstStat);
	cmdList->ResourceBarrier(1, &br);
}
