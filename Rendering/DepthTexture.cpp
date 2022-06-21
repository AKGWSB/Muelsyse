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
	GraphicContex::g_dsvHeap->FreeDescriptor(dsvHandleIndex);
}

// global resource pool, find by name
std::map<std::string, std::unique_ptr<DepthTexture>> DepthTexture::g_depthTextureResourceMap;
DepthTexture* DepthTexture::Find(std::string name)
{
	if (g_depthTextureResourceMap.find(name) == g_depthTextureResourceMap.end())
	{
		return NULL;
	}
	return g_depthTextureResourceMap[name].get();
}

DepthTexture* DepthTexture::Create(std::string name, int _width, int _height)
{
	g_depthTextureResourceMap[name] = std::make_unique<DepthTexture>(_width, _height);
	return g_depthTextureResourceMap[name].get();
}

void DepthTexture::FreeAll()
{
	for (auto& p : g_depthTextureResourceMap)
	{
		delete p.second.release();
	}
}
