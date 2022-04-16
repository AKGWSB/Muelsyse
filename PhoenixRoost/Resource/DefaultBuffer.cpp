#include "DefaultBuffer.h"

DefaultBuffer::DefaultBuffer(ComPtr<ID3D12Device> dv, std::wstring bn)
{
	device = dv;
    bufferName = bn;
}

DefaultBuffer::~DefaultBuffer()
{
    // We should release the memory we allocated for our buffer on the GPU
    SAFE_RELEASE(buffer.Get());
}

void DefaultBuffer::UploadData(const void* data, UINT size)
{
    bufferSize = size;

    // create default heap
    auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
    ThrowIfFailed(device->CreateCommittedResource(
        &heapProperties,                // a default heap
        D3D12_HEAP_FLAG_NONE,           // no flags
        &resourceDesc,                  // resource description for a buffer
        D3D12_RESOURCE_STATE_COPY_DEST, // start in the copy destination state
        nullptr,
        IID_PPV_ARGS(&buffer)));
    buffer->SetName(bufferName.c_str());


    // create upload heap
    ComPtr<ID3D12Resource> uploadBuffer;

    auto uHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto uResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
    ThrowIfFailed(device->CreateCommittedResource(
        &uHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &uResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&uploadBuffer)));


    // create temp alloctor, list and queue for copy command
    ComPtr<ID3D12CommandAllocator> tempCommandAllocator;
    ComPtr<ID3D12GraphicsCommandList> tempCommandList;
    ComPtr<ID3D12CommandQueue> tempCommandQueue;

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.NodeMask = 0;

    ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&tempCommandAllocator)));
    ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, tempCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&tempCommandList)));
    ThrowIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&tempCommandQueue)));
    

    // send data to upload buffer
    UINT8* pVertexDataBegin;
    CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
    ThrowIfFailed(uploadBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
    memcpy(pVertexDataBegin, data, bufferSize);
    uploadBuffer->Unmap(0, nullptr);


    // copy from upload buffer to default buffer
    tempCommandList->CopyBufferRegion(buffer.Get(), 0, uploadBuffer.Get(), 0, bufferSize);

    // change buffer state
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    tempCommandList->ResourceBarrier(1, &barrier);

    // exe cmd
    tempCommandList->Close();
    std::vector<ID3D12CommandList*> ppCommandLists{ tempCommandList.Get() };
    tempCommandQueue->ExecuteCommandLists(static_cast<UINT>(ppCommandLists.size()), ppCommandLists.data());

    // set fence
    UINT64 initialValue{ 0 };
    ComPtr<ID3D12Fence> tempFence;
    ThrowIfFailed(device->CreateFence(initialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&tempFence)));

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
