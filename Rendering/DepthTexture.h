#pragma once

#include "../Resource/Texture2D.h"

class DepthTexture : public Texture2D
{
public:
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvCpuHandle;		// cpu handle for dsv descriptor
	CD3DX12_GPU_DESCRIPTOR_HANDLE dsvGpuHandle;		// gpu handle for dsv descriptor
	UINT dsvHandleIndex;							// index in dsv descriptor heap

	DepthTexture(int _width, int _height);
	~DepthTexture();

	// global resource pool, find by name
	static std::map<std::string, std::unique_ptr<DepthTexture>> g_depthTextureResourceMap;
	static DepthTexture* Create(std::string name, int _width, int _height);
	static DepthTexture* Find(std::string name);
	static void FreeAll();
};
