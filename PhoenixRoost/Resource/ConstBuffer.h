#pragma once

#include <initguid.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>

#include "../DX12/d3dx12.h"
#include "../DX12/DirectXHelper.h"
#include "../DX12/DescriptorHeap.h"

class ConstBuffer
{
public:
	ID3D12Device* device;
	UINT bufferSize;
	std::wstring bufferName;
	ComPtr<ID3D12Resource> buffer;

	DescriptorHeap* cbvHeap;
	CD3DX12_CPU_DESCRIPTOR_HANDLE cbvCpuHandle;		// cpu handle for cbv descriptor
	CD3DX12_GPU_DESCRIPTOR_HANDLE cbvGpuHandle;		// gpu handle for cbv descriptor
	UINT cbvHandleIndex;							// index in cbv descriptor heap

	// the GPU address mapped into CPU RAM
	// change in this address will mapped to GPU memory
	UINT8* mappedCpuAddress;

	ConstBuffer(ID3D12Device* dv, DescriptorHeap* g_cbvHeap, std::wstring bn = L"buffer", UINT bs = 4096);
	~ConstBuffer();

	void UpdateData(const void* pdata, UINT size);
};

