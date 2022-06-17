#include "DescriptorHeap.h"
#include "GraphicContex.h"
#include "helper.h"

#include <exception>

DescriptorHeap::DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, D3D12_DESCRIPTOR_HEAP_FLAGS heapFlags, UINT hSize)
{
    // init data
    descriptorNum = 0;
	heapSize = hSize;
    freeList = std::vector<int>(heapSize, 0);
    descriptorSize = GraphicContex::g_device->GetDescriptorHandleIncrementSize(heapType);

    // Create descriptor heaps.
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = heapSize;
    heapDesc.Type = heapType;
    heapDesc.Flags = heapFlags;
    ThrowIfFailed(GraphicContex::g_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap)));
}

DescriptorHeap::~DescriptorHeap()
{
    //OutputDebugString(L"Descriptor Heap De Construct!\n");
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
        throw std::exception();
    }

    descriptorNum += 1;
    return id;
}

bool DescriptorHeap::FreeDescriptor(UINT id)
{
    if (id >= heapSize || freeList[id] == 0)
    {
        return false;
    }

    freeList[id] = 0;

    descriptorNum -= 1;
    return true;
}

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
