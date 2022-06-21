#pragma once

#include <memory>
#include "../Resource/Texture2D.h"

class RenderTexture : public Texture2D
{
public:
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvCpuHandle;		// cpu handle for rtv descriptor
	CD3DX12_GPU_DESCRIPTOR_HANDLE rtvGpuHandle;		// gpu handle for rtv descriptor
	UINT rtvHandleIndex;							// index in rtv descriptor heap

	RenderTexture(int _width, int _height, DXGI_FORMAT _format);
	~RenderTexture();

	void ChangeToRenderTargetState();
	void ChangeToShaderRsourceState();

	// global resource pool, find by name
	static std::map<std::string, std::unique_ptr<RenderTexture>> g_renderTextureResourceMap;
	static RenderTexture* Create(std::string name, int _width, int _height, DXGI_FORMAT _format);
	static RenderTexture* Find(std::string name);
	static void FreeAll();
};
