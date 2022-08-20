#pragma once

#include "Texture2D.h"

class DepthTexture
{
private:
	Descriptor m_dsvDescriptor;
	Descriptor m_srvDescriptor;
	ComPtr<ID3D12Resource> m_buffer;
	DXGI_FORMAT m_format;

public:
	std::string name;
	int width;
	int height;

	DepthTexture(int w, int h);
	~DepthTexture();

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetDsvCpuHandle();
};

