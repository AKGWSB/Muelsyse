#pragma once

#include "../DX12/d3dx12.h"
#include "../DX12/DirectXHelper.h"

class DescriptorHeap
{
public:
	UINT heapSize;
	ComPtr<ID3D12DescriptorHeap> heap;
	UINT descriptorSize;
	std::vector<int> freeList;

	DescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE hType, D3D12_DESCRIPTOR_HEAP_FLAGS heapFlags, UINT hSize = 1024);
	~DescriptorHeap();

	UINT AllocDescriptor();
	bool FreeDescriptor(UINT id);
	//ID3D12DescriptorHeap* GetHeap();

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(UINT id);
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(UINT id);
};

