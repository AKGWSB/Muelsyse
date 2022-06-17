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
};
