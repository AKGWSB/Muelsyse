#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <string>

#include "../Core/DescriptorManager.h"

using Microsoft::WRL::ComPtr;

class Texture2D
{
private:
	Descriptor m_srvDescriptor;
	ComPtr<ID3D12Resource> m_buffer;
	DXGI_FORMAT m_format;

	void CreateEmpty(int w, int h, DXGI_FORMAT fmt);
	void LoadFromData(int w, int h, DXGI_FORMAT fmt, void* pData, UINT pixelByteSize);
	void LoadFromFile(std::string filepath);

public:
	std::string name;
	int width;
	int height;

	Texture2D(std::string filepath);
	~Texture2D();

	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuHandle();
};
