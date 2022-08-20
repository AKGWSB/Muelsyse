#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <string>

#include "../Core/DescriptorManager.h"

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

