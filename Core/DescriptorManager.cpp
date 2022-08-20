#include "DescriptorManager.h"
#include "GraphicContex.h"
#include "helper.h"

#include <exception>

// ---------------------------------------------------------------------- //

DescriptorHeap::DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, D3D12_DESCRIPTOR_HEAP_FLAGS heapFlags, UINT hSize)
{
    ID3D12Device* g_device = GraphicContex::GetInstance()->GetDevice();

    // init data
    m_type = heapType;
    for (int i = 0; i < hSize; i++)
    {
        m_freeIndexPool.insert(i);
    }
    m_incrementSize = g_device->GetDescriptorHandleIncrementSize(heapType);

    // Create descriptor heaps.
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = hSize;
    heapDesc.Type = heapType;
    heapDesc.Flags = heapFlags;
    ThrowIfFailed(g_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_heap)));
}

DescriptorHeap::~DescriptorHeap()
{
    //OutputDebugStringA("DescriptorHeap de cons\n");
}

Descriptor DescriptorHeap::Alloc()
{
    // check
    if (m_freeIndexPool.size() == 0)
    {
        throw("Runtime Error: Descriptor Heap Over Size!");
    }

    // alloc an unique index
    int index = *(m_freeIndexPool.begin());
    m_freeIndexPool.erase(index);

    Descriptor desc;
    desc.type = m_type;
    desc.index = index;
    desc.cpuHandle = GetCpuHandle(index);

    if (!(m_type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV) && !(m_type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV))
    {
        desc.gpuHandle = GetGpuHandle(index);
    }

    return desc;
}

void DescriptorHeap::Free(Descriptor desc)
{
    m_freeIndexPool.insert(desc.index);
}

ID3D12DescriptorHeap* DescriptorHeap::GetD3DHeap()
{
    return m_heap.Get();
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCpuHandle(UINT id)
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE handle;
    handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_heap->GetCPUDescriptorHandleForHeapStart());
    handle.Offset(id, m_incrementSize);

    return handle;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGpuHandle(UINT id)
{
    CD3DX12_GPU_DESCRIPTOR_HANDLE handle;
    handle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_heap->GetGPUDescriptorHandleForHeapStart());
    handle.Offset(id, m_incrementSize);

    return handle;
}

// ---------------------------------------------------------------------- //

DescriptorManager::DescriptorManager()
{
    m_descriptorHeapMap[D3D12_DESCRIPTOR_HEAP_TYPE_RTV] = std::make_unique<DescriptorHeap>(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
    m_descriptorHeapMap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] = std::make_unique<DescriptorHeap>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
    m_descriptorHeapMap[D3D12_DESCRIPTOR_HEAP_TYPE_DSV] = std::make_unique<DescriptorHeap>(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
    m_descriptorHeapMap[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER] = std::make_unique<DescriptorHeap>(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 64);
}

DescriptorManager::~DescriptorManager()
{ 
    //OutputDebugStringA("DescriptorManager de cons\n");
}

DescriptorManager* DescriptorManager::GetInstance()
{
    static DescriptorManager instance;
    return &instance;
}

Descriptor DescriptorManager::AllocDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type)
{
    return m_descriptorHeapMap[type]->Alloc();
}

void DescriptorManager::FreeDescriptor(Descriptor desc)
{
    m_descriptorHeapMap[desc.type]->Free(desc);
}