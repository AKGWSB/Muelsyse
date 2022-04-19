#include "ConstBuffer.h"

ConstBuffer::ConstBuffer(ID3D12Device* dv, DescriptorHeap* g_cbvHeap, std::wstring bn, UINT bs)
{
	device = dv;
    cbvHeap = g_cbvHeap;
	bufferName = bn;
	bufferSize = bs;
    mappedCpuAddress = nullptr;

	// CB size is required to be 256-byte aligned.
	if (bufferSize % 256)
	{
		bufferSize = (bufferSize / 256 + 1) * 256;
	}



    // alloc cbv descriptor
    cbvHandleIndex = cbvHeap->AllocDescriptor();
    cbvCpuHandle = cbvHeap->GetCpuHandle(cbvHandleIndex);
    cbvGpuHandle = cbvHeap->GetGpuHandle(cbvHandleIndex);



    // Create the constant buffer.
    auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
    ThrowIfFailed(device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&buffer)));

    // Describe and create a constant buffer view.
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = buffer->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = bufferSize;
    device->CreateConstantBufferView(&cbvDesc, cbvCpuHandle);

    // Map and initialize the constant buffer. We don't unmap this until the
    // app closes. Keeping things mapped for the lifetime of the resource is okay.
    CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
    ThrowIfFailed(buffer->Map(0, &readRange, reinterpret_cast<void**>(&mappedCpuAddress)));
}

ConstBuffer::~ConstBuffer()
{
    // We should release the memory we allocated for our buffer on the GPU
    SAFE_RELEASE(buffer.Get());

    // release descriptor
    cbvHeap->FreeDescriptor(cbvHandleIndex);

    // release memory mapping
    //buffer->Unmap(0, nullptr);
}

void ConstBuffer::UpdateData(const void* pdata, UINT size)
{
    memcpy(mappedCpuAddress, pdata, size);
}
