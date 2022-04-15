#include "DescriptorHeap.h"

DescriptorHeap::DescriptorHeap(ID3D12Device* m_device, D3D12_DESCRIPTOR_HEAP_TYPE hType, uint32_t hSize)
{
    // init data
	heapType = hType;
	heapSize = hSize;
    freeList = std::vector<int>(heapSize, 0);
    descriptorSize = m_device->GetDescriptorHandleIncrementSize(hType);

    // Create descriptor heaps.
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = heapSize;
    heapDesc.Type = heapType;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap)));
}

DescriptorHeap::~DescriptorHeap()
{

}

bool DescriptorHeap::AllocDescriptor(CD3DX12_CPU_DESCRIPTOR_HANDLE& handle, UINT& id)
{
    // find first empty place
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
        id = 0;
        return false;
    }

    // alloc handle
    handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(heap->GetCPUDescriptorHandleForHeapStart());
    handle.Offset(id, descriptorSize);

    return true;
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
