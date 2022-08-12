#include "UploadBuffer.h"
#include "../Core/helper.h"
#include "../Core/GraphicContex.h"

UploadBuffer::UploadBuffer(UINT size)
{
    ID3D12Device* device = GraphicContex::GetInstance()->GetDevice();
    DescriptorManager* descManager = DescriptorManager::GetInstance();

    if ((size % 256) != 0)
    {
        size = (size / 256 + 1) * 256;
    }
    m_size = size;

    m_bufferData.resize(m_size);

    // create buffer
    auto heapProp1 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto resDesc1 = CD3DX12_RESOURCE_DESC::Buffer(m_size);
    ThrowIfFailed(device->CreateCommittedResource(
        &heapProp1,
        D3D12_HEAP_FLAG_NONE,
        &resDesc1,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_buffer)));

    // alloc cbv descriptor
    m_cbvDescriptor = descManager->AllocDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    // Describe and create a constant buffer view.
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = m_buffer->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = m_size;
    device->CreateConstantBufferView(&cbvDesc, m_cbvDescriptor.cpuHandle);

    // Map and initialize the constant buffer. We don't unmap this until the
    // app closes. Keeping things mapped for the lifetime of the resource is okay.
    CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
    ThrowIfFailed(m_buffer->Map(0, &readRange, reinterpret_cast<void**>(&m_mappedGpuAddress)));
}

UploadBuffer::~UploadBuffer()
{
    auto descManager = DescriptorManager::GetInstance();
    descManager->FreeDescriptor(m_cbvDescriptor);

    m_buffer->Unmap(0, nullptr);
}

void UploadBuffer::UpdateSubData(UINT startByte, UINT size, void* pData)
{
    memcpy(m_bufferData.data() + startByte, pData, size);
}

void UploadBuffer::Upload()
{
    memcpy(m_mappedGpuAddress, m_bufferData.data(), m_size);
}

CD3DX12_GPU_DESCRIPTOR_HANDLE UploadBuffer::GetGpuHandle()
{
    return m_cbvDescriptor.gpuHandle;
}
