#pragma once

#include <initguid.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>

#include "../DX12/d3dx12.h"
#include "../DX12/DirectXHelper.h"

#include "DefaultBuffer.h"
#include "../DX12/DescriptorHeap.h"

class Texture2D
{
public:
	int width;
	int height;

	DescriptorHeap* srvHeap;
	DescriptorHeap* samplerHeap;

	ComPtr<ID3D12Resource> buffer;

	CD3DX12_CPU_DESCRIPTOR_HANDLE srvCpuHandle;		// cpu handle for srv descriptor
	CD3DX12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;		// gpu handle for srv descriptor
	UINT srvHandleIndex;							// index in srv descriptor heap

	CD3DX12_CPU_DESCRIPTOR_HANDLE samplerCpuHandle;	// cpu handle for sampler descriptor
	CD3DX12_GPU_DESCRIPTOR_HANDLE samplerGpuHandle;	// cpu handle for sampler descriptor
	UINT samplerHandleIndex;						// index in sampler descriptor heap

	Texture2D(ID3D12Device* device, DescriptorHeap* g_srvHeap, DescriptorHeap* g_samplerHeap, std::string texturePath);
	~Texture2D();
};

