#include "DepthTexture.h"
#include "../Core/GraphicContex.h"

DepthTexture::DepthTexture(int _width, int _height) :
	// create texture
	Texture2D(
		_width, _height, DXGI_FORMAT_D24_UNORM_S8_UINT,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, D3D12_RESOURCE_STATE_DEPTH_WRITE)
{
	// alloc descriptor from global heap
	dsvHandleIndex = GraphicContex::g_dsvHeap->AllocDescriptor();
	dsvCpuHandle = GraphicContex::g_dsvHeap->GetCpuHandle(dsvHandleIndex);
	//dsvGpuHandle = GraphicContex::g_dsvHeap->GetGpuHandle(dsvHandleIndex);

	// create dsv
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	GraphicContex::g_device->CreateDepthStencilView(buffer.Get(), &depthStencilDesc, dsvCpuHandle);
}

DepthTexture::~DepthTexture()
{

}
