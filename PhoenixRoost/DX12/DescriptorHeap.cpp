#include "DescriptorHeap.h"

DescriptorHeap::DescriptorHeap(ID3D12Device* m_device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, D3D12_DESCRIPTOR_HEAP_FLAGS heapFlags, UINT hSize)
{
    // init data
	heapSize = hSize;
    freeList = std::vector<int>(heapSize, 0);
    descriptorSize = m_device->GetDescriptorHandleIncrementSize(heapType);

    // Create descriptor heaps.
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = heapSize;
    heapDesc.Type = heapType;
    heapDesc.Flags = heapFlags;
    ThrowIfFailed(m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap)));
}

DescriptorHeap::~DescriptorHeap()
{

}

UINT DescriptorHeap::AllocDescriptor()
{
    // find first empty place
    UINT id;
    for (id = 0; id < heapSize; id++)
    {
        if (freeList[id] == 0)
        {
            freeList[id] = 1;
            break;
        }
    }

    // run out of descriptor
    if (id == heapSize)
    {
        return 0;
    }

    return id;
}

bool DescriptorHeap::FreeDescriptor(UINT id)
{
    if (id >= heapSize || freeList[id] == 0)
    {
        return false;
    }

    freeList[id] = 0;

    return true;
}

/*
ID3D12DescriptorHeap* DescriptorHeap::GetHeap()
{
    return heap.Get();
}
*/

CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCpuHandle(UINT id)
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE handle;
    handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(heap->GetCPUDescriptorHandleForHeapStart());
    handle.Offset(id, descriptorSize);

    return handle;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGpuHandle(UINT id)
{
    CD3DX12_GPU_DESCRIPTOR_HANDLE handle;
    handle = CD3DX12_GPU_DESCRIPTOR_HANDLE(heap->GetGPUDescriptorHandleForHeapStart());
    handle.Offset(id, descriptorSize);

    return handle;
}
