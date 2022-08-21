#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <string>

#include "Texture2D.h"

#include "../Core/DescriptorManager.h"

class DepthTexture : public Texture2D
{
private:
	Descriptor m_dsvDescriptor;

public:
	std::string name;
	int width;
	int height;

	DepthTexture(int w, int h);
	~DepthTexture();

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetDsvCpuHandle();
};

