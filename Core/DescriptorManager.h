#pragma once

#include <unordered_set>
#include <map>
#include <vector>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

// ---------------------------------------------------------------------- //

struct Descriptor
{
	int index;	// unique index in ID3D12DescriptorHeap
	D3D12_DESCRIPTOR_HEAP_TYPE type;
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle;
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle;
};

// ---------------------------------------------------------------------- //

// allow to assign descriptor dynamic
class DescriptorHeap
{
private:
	D3D12_DESCRIPTOR_HEAP_TYPE m_type;
	UINT m_incrementSize;
	ComPtr<ID3D12DescriptorHeap> m_heap;

	// storage descriptor's unique index in ID3D12DescriptorHeap
	std::unordered_set<int> m_freeIndexPool;

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(UINT id);
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(UINT id);

public:
	DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, D3D12_DESCRIPTOR_HEAP_FLAGS heapFlags, UINT hSize = 4096);
	~DescriptorHeap();

	Descriptor Alloc();
	void Free(Descriptor desc);

	// get native heap
	ID3D12DescriptorHeap* GetD3DHeap();
};

// ---------------------------------------------------------------------- //

class DescriptorManager
{
	friend class GraphicContex;

private:
	DescriptorManager();
	DescriptorManager(const DescriptorManager&);
	DescriptorManager& operator=(const DescriptorManager&);

	// find descriptor heaps by type
	std::map<D3D12_DESCRIPTOR_HEAP_TYPE, std::unique_ptr<DescriptorHeap>> m_descriptorHeapMap;

public:
	~DescriptorManager();

	// single ton
	static DescriptorManager* GetInstance();

	Descriptor AllocDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type);
	void FreeDescriptor(Descriptor desc);
};
