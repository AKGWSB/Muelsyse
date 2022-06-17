#include "UploadBuffer.h"
#include "../Core/helper.h"
#include "../Core/d3dx12.h"
#include "../Core/GraphicContex.h"

#include <D3Dcompiler.h>

UploadBuffer::UploadBuffer()
{
    CreateBuffer(1024);
}

UploadBuffer::~UploadBuffer()
{

}

void UploadBuffer::CreateBuffer(UINT size)
{
    // alloc descriptor from global heap
    cbvHandleIndex = GraphicContex::g_srvHeap->AllocDescriptor();
    cbvCpuHandle = GraphicContex::g_srvHeap->GetCpuHandle(cbvHandleIndex);
    cbvGpuHandle = GraphicContex::g_srvHeap->GetGpuHandle(cbvHandleIndex);

    // CB size is required to be 256-byte aligned.
    UINT constantBufferSize = sizeof(size);    
    if (constantBufferSize % 256 != 0)
    {
        constantBufferSize = (constantBufferSize / 256 + 1) * 256;
    }

    // r
    bufferSize = constantBufferSize;
    data.resize(bufferSize);

    // create buffer
    auto u = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto v = CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize);
    ThrowIfFailed(GraphicContex::g_device->CreateCommittedResource(
        &u,
        D3D12_HEAP_FLAG_NONE,
        &v,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&buffer)));

    // Describe and create a constant buffer view.
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = buffer->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = constantBufferSize;
    GraphicContex::g_device->CreateConstantBufferView(&cbvDesc, cbvCpuHandle);
}

void UploadBuffer::Upload()
{
    // upload
    BYTE* cpuDestData;                          // copy to this memory

    // Map and initialize the constant buffer. We don't unmap this until the
    // app closes. Keeping things mapped for the lifetime of the resource is okay.
    CD3DX12_RANGE readRange(0, 0);          // We do not intend to read from this resource on the CPU.
    ThrowIfFailed(buffer->Map(0, &readRange, reinterpret_cast<void**>(&cpuDestData)));
    memcpy(cpuDestData, data.data(), bufferSize);
    buffer->Unmap(0, &readRange);
}

void UploadBuffer::UpdateSubData(UINT start, UINT size, void* src)
{
    memcpy(data.data() + start, src, size);
}
