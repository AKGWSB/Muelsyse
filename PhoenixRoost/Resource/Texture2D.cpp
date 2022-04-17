#include "Texture2D.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../3rdparty/stb_image.h"

Texture2D::Texture2D(ID3D12Device* device, DescriptorHeap* g_srvHeap, std::string texturePath)
{
    srvHeap = g_srvHeap;

	// load data as rgba format (4 channels)
    int nChannels;
	unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nChannels, 4);
	UINT bufferSize = width * height * 4;



    // alloc a srv descriptor from srv heap
    srvHandleIndex = srvHeap->AllocDescriptor();
    srvCpuHandle = srvHeap->GetCpuHandle(srvHandleIndex);
    srvGpuHandle = srvHeap->GetGpuHandle(srvHandleIndex);



    // Describe and create a Texture2D.
    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    textureDesc.DepthOrArraySize = 1;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

    // Create the GPU default buffer.
    auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &textureDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&buffer)));

    // Create the GPU upload buffer.
    ComPtr<ID3D12Resource> uploadBuffer;
    auto uheapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto uResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
    ThrowIfFailed(device->CreateCommittedResource(
        &uheapProperties,
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



    // store buffer in upload heap
    D3D12_SUBRESOURCE_DATA subData = {};
    subData.pData = data;                 // pointer to our index array
    subData.RowPitch = bufferSize;        // size of all our index buffer
    subData.SlicePitch = bufferSize;      // also the size of our index buffer

    UpdateSubresources(tempCommandList.Get(), buffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subData);

    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    tempCommandList->ResourceBarrier(1, &barrier);



    // Describe and create a SRV for the texture.
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    device->CreateShaderResourceView(buffer.Get(), &srvDesc, srvCpuHandle);
    //device->CreateShaderResourceView(buffer.Get(), &srvDesc, srvHeap->heap->GetCPUDescriptorHandleForHeapStart());


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

Texture2D::~Texture2D()
{
    // We should release the memory we allocated for our buffer on the GPU
    SAFE_RELEASE(buffer.Get());

    // release descriptor
    srvHeap->FreeDescriptor(srvHandleIndex);
}
