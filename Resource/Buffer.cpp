#include "Buffer.h"
#include "../Core/d3dx12.h"
#include "../Core/GraphicContex.h"
#include "../Core/helper.h"

Buffer::Buffer()
{

}

Buffer::~Buffer()
{

}

void Buffer::CreateBuffer(UINT size)
{
    bufferSize = size;
    data.resize(bufferSize);

    // create default heap
    auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
    ThrowIfFailed(GraphicContex::g_device->CreateCommittedResource(
        &heapProperties,                // a default heap
        D3D12_HEAP_FLAG_NONE,           // no flags
        &resourceDesc,                  // resource description for a buffer
        D3D12_RESOURCE_STATE_COPY_DEST, // start in the copy destination state
        nullptr,
        IID_PPV_ARGS(&buffer)));

    // create upload heap
    auto uHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto uResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
    ThrowIfFailed(GraphicContex::g_device->CreateCommittedResource(
        &uHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &uResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&tempUploadBuffer)));


    // create temp alloctor, list and queue for copy command
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.NodeMask = 0;

    ThrowIfFailed(GraphicContex::g_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&tempCommandAllocator)));
    ThrowIfFailed(GraphicContex::g_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, tempCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&tempCommandList)));
    ThrowIfFailed(GraphicContex::g_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&tempCommandQueue)));
}

void Buffer::UpdateSubData(UINT start, UINT size, void* pdata)
{
    memcpy(&data[0] + start, pdata, size);
}

void Buffer::Upload()
{
    // change buffer state
    //auto to_copyDest = CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_COPY_DEST);
    //tempCommandList->ResourceBarrier(1, &to_copyDest);

    // upload
    D3D12_SUBRESOURCE_DATA subDataDesc = {};
    subDataDesc.pData = data.data();          // pointer to our index array
    subDataDesc.RowPitch = bufferSize;        // size of all our index buffer
    subDataDesc.SlicePitch = bufferSize;      // also the size of our index buffer

    UpdateSubresources(tempCommandList.Get(), buffer.Get(), tempUploadBuffer.Get(), 0, 0, 1, &subDataDesc);

    // change buffer state
    auto to_buffer = CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    tempCommandList->ResourceBarrier(1, &to_buffer);

    // exe cmd
    tempCommandList->Close();
    std::vector<ID3D12CommandList*> ppCommandLists{ tempCommandList.Get() };
    tempCommandQueue->ExecuteCommandLists(static_cast<UINT>(ppCommandLists.size()), ppCommandLists.data());

    // set fence
    UINT64 initialValue{ 0 };
    ComPtr<ID3D12Fence> tempFence;
    ThrowIfFailed(GraphicContex::g_device->CreateFence(initialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&tempFence)));

    HANDLE tempFenceEvent{ CreateEvent(nullptr, FALSE, FALSE, nullptr) };
    if (tempFenceEvent == NULL)
    {
        throw("Error creating a fence event.");
    }

    // wait for gpu
    ThrowIfFailed(tempCommandQueue->Signal(tempFence.Get(), 1));
    ThrowIfFailed(tempFence->SetEventOnCompletion(1, tempFenceEvent));
    WaitForSingleObjectEx(tempFenceEvent, INFINITE, FALSE);
}

D3D12_GPU_VIRTUAL_ADDRESS Buffer::GetGpuAddress()
{
    return buffer->GetGPUVirtualAddress();
}