#include "Texture2D.h"
#include "../Core/helper.h"
#include "../Core/GraphicContex.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#include "../Library/stb_image.h"

Texture2D::Texture2D(int _width, int _height, DXGI_FORMAT _format, D3D12_RESOURCE_FLAGS _flags, D3D12_RESOURCE_STATES _state)
{
    // alloc descriptor from global heap
    srvHandleIndex = GraphicContex::g_srvHeap->AllocDescriptor();
    srvCpuHandle = GraphicContex::g_srvHeap->GetCpuHandle(srvHandleIndex);
    srvGpuHandle = GraphicContex::g_srvHeap->GetGpuHandle(srvHandleIndex);

    samplerHandleIndex = GraphicContex::g_splHeap->AllocDescriptor();
    samplerCpuHandle = GraphicContex::g_splHeap->GetCpuHandle(samplerHandleIndex);
    samplerGpuHandle = GraphicContex::g_splHeap->GetGpuHandle(samplerHandleIndex);

    width = _width;
    height = _height;
    format = _format;

    // special for depth buffer 
    bool isDepthTex = (format == DXGI_FORMAT_D24_UNORM_S8_UINT);

    // Describe and create a Texture2D.
    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels = 1;
    textureDesc.Format = format;
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.Flags = _flags;
    textureDesc.DepthOrArraySize = 1;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

    D3D12_CLEAR_VALUE clearValue = {};
    clearValue.Format = textureDesc.Format;
    clearValue.Color[0] = 0.5f;
    clearValue.Color[1] = 0.5f;
    clearValue.Color[2] = 0.5f;
    clearValue.Color[3] = 1.0f;
    if (isDepthTex)
    {
        clearValue.DepthStencil.Depth = 1.0f;
        clearValue.DepthStencil.Stencil = 0;
    }

    auto hp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(GraphicContex::g_device->CreateCommittedResource(
        &hp,
        D3D12_HEAP_FLAG_NONE,
        &textureDesc,
        _state,
        &clearValue,
        IID_PPV_ARGS(&buffer)));

    // Describe and create a SRV for the texture.
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = isDepthTex ? DXGI_FORMAT_R24_UNORM_X8_TYPELESS : textureDesc.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    GraphicContex::g_device->CreateShaderResourceView(buffer.Get(), &srvDesc, srvCpuHandle);

    // create sampler descriptor
    D3D12_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    GraphicContex::g_device->CreateSampler(&samplerDesc, samplerCpuHandle);
}

Texture2D::Texture2D(std::string filepath)
{
	LoadFromFile(filepath);
}

Texture2D::~Texture2D()
{
    //GraphicContex::g_srvHeap->FreeDescriptor(srvHandleIndex);
    //GraphicContex::g_splHeap->FreeDescriptor(samplerHandleIndex);
}

void Texture2D::LoadFromFile(std::string filepath)
{
    name = filepath;

    // alloc descriptor from global heap
    srvHandleIndex = GraphicContex::g_srvHeap->AllocDescriptor();
    srvCpuHandle = GraphicContex::g_srvHeap->GetCpuHandle(srvHandleIndex);
    srvGpuHandle = GraphicContex::g_srvHeap->GetGpuHandle(srvHandleIndex);

    samplerHandleIndex = GraphicContex::g_splHeap->AllocDescriptor();
    samplerCpuHandle = GraphicContex::g_splHeap->GetCpuHandle(samplerHandleIndex);
    samplerGpuHandle = GraphicContex::g_splHeap->GetGpuHandle(samplerHandleIndex);

    // create temp alloctor, list and queue for copy command
    ComPtr<ID3D12CommandAllocator> tempCommandAllocator;
    ComPtr<ID3D12GraphicsCommandList> tempCommandList;
    ComPtr<ID3D12CommandQueue> tempCommandQueue;

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.NodeMask = 0;

    ThrowIfFailed(GraphicContex::g_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&tempCommandAllocator)));
    ThrowIfFailed(GraphicContex::g_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, tempCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&tempCommandList)));
    ThrowIfFailed(GraphicContex::g_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&tempCommandQueue)));

    // read data from file
    int nChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nChannels, 4);

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

    auto hp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(GraphicContex::g_device->CreateCommittedResource(
        &hp,
        D3D12_HEAP_FLAG_NONE,
        &textureDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&buffer)));

    // warning : for texture2D, buffer size always bigger than 1D buffer
    // using [size = width * height * 4] will cause some problems ...
    const UINT64 uploadBufferSize = GetRequiredIntermediateSize(buffer.Get(), 0, 1);

    // Create the GPU upload buffer.
    ComPtr<ID3D12Resource> textureUploadHeap;
    auto uhp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto ubf = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
    ThrowIfFailed(GraphicContex::g_device->CreateCommittedResource(
        &uhp,
        D3D12_HEAP_FLAG_NONE,
        &ubf,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&textureUploadHeap)));

    // Copy data to the intermediate upload heap and then schedule a copy from the upload heap to the Texture2D.
    const int TexturePixelSize = 4; // RGBA 

    D3D12_SUBRESOURCE_DATA textureData = {};
    textureData.pData = data;
    textureData.RowPitch = width * TexturePixelSize;
    textureData.SlicePitch = textureData.RowPitch * height;

    UpdateSubresources(tempCommandList.Get(), buffer.Get(), textureUploadHeap.Get(), 0, 0, 1, &textureData);
    auto br = CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    tempCommandList->ResourceBarrier(1, &br);

    // Describe and create a SRV for the texture.
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    GraphicContex::g_device->CreateShaderResourceView(buffer.Get(), &srvDesc, srvCpuHandle);

    // create sampler descriptor
    D3D12_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    GraphicContex::g_device->CreateSampler(&samplerDesc, samplerCpuHandle);

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

    delete[] data;
}

void Texture2D::Create(int _width, int _height, DXGI_FORMAT _format, void* data)
{

}

// global resource pool, find by filename
std::map<std::string, std::unique_ptr<Texture2D>> Texture2D::g_textureResourceMap;
Texture2D* Texture2D::Find(std::string filepath)
{
    if (g_textureResourceMap.find(filepath) == g_textureResourceMap.end())
    {
        g_textureResourceMap[filepath] = std::make_unique<Texture2D>(filepath);
    }
    return g_textureResourceMap[filepath].get();
}