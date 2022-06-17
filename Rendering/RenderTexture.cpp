#include "RenderTexture.h"
#include "../Core/GraphicContex.h"

// normal render texture
RenderTexture::RenderTexture(int _width, int _height, DXGI_FORMAT _format) :
	// create texture
	Texture2D(
		_width, _height, _format, 
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)	
{
	// alloc descriptor from global heap
	rtvHandleIndex = GraphicContex::g_rtvHeap->AllocDescriptor();
	rtvCpuHandle = GraphicContex::g_rtvHeap->GetCpuHandle(rtvHandleIndex);
	//rtvGpuHandle = GraphicContex::g_rtvHeap->GetGpuHandle(rtvHandleIndex);

	// create rtv
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = _format;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Texture2D.PlaneSlice = 0;

	GraphicContex::g_device->CreateRenderTargetView(buffer.Get(), &rtvDesc, rtvCpuHandle);
}

RenderTexture::~RenderTexture()
{
	//GraphicContex::g_rtvHeap->FreeDescriptor(rtvHandleIndex);
}

void RenderTexture::ChangeToRenderTargetState()
{
	auto br = CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	GraphicContex::g_commandList->ResourceBarrier(1, &br);
}

void RenderTexture::ChangeToShaderRsourceState()
{
	auto br = CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	GraphicContex::g_commandList->ResourceBarrier(1, &br);
}