#pragma once

#include "../stdafx.h"
#include <vector>

class DescriptorHeap
{
public:
	D3D12_DESCRIPTOR_HEAP_TYPE heapType;
	uint32_t heapSize;
	ComPtr<ID3D12DescriptorHeap> heap;
	UINT descriptorSize;
	std::vector<int> freeList;

	DescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE hType, uint32_t hSize = 1024);
	~DescriptorHeap();

	bool AllocDescriptor(CD3DX12_CPU_DESCRIPTOR_HANDLE& handle, UINT& id);
	bool FreeDescriptor(UINT id);
};

