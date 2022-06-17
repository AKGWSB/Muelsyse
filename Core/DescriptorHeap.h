#pragma once

#include "d3dx12.h"

#include <vector>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

using Microsoft::WRL::ComPtr;

class DescriptorHeap
{
public:
	UINT heapSize;
	ComPtr<ID3D12DescriptorHeap> heap;
	UINT descriptorSize;
	std::vector<int> freeList;
	UINT descriptorNum;

	DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, D3D12_DESCRIPTOR_HEAP_FLAGS heapFlags, UINT hSize = 1024);
	~DescriptorHeap();

	UINT AllocDescriptor();
	bool FreeDescriptor(UINT id);

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(UINT id);
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(UINT id);
};

