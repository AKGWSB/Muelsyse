#include "Texture2D.h"
#include "../Core/GraphicContex.h"
#include "../Core/helper.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#include "../Library/stb_image.h"

Texture2D::Texture2D(int w, int h, DXGI_FORMAT fmt, D3D12_RESOURCE_FLAGS flag)
{
    CreateEmpty(w, h, fmt, flag);
}

Texture2D::Texture2D(std::string filepath)
{
    LoadFromFile(filepath);
}

Texture2D::~Texture2D()
{
    //OutputDebugStringA("Texture2D de cons\n");
	DescriptorManager* descManager = DescriptorManager::GetInstance();
	descManager->FreeDescriptor(m_srvDescriptor);
}

void Texture2D::CreateEmpty(int w, int h, DXGI_FORMAT fmt, D3D12_RESOURCE_FLAGS flag)
{
    DescriptorManager* descManager = DescriptorManager::GetInstance();
    GraphicContex* contex = GraphicContex::GetInstance();
    ID3D12Device* device = contex->GetDevice();

    width = w;
    height = h;
    m_format = fmt;
    m_flag = flag;

    // alloc d
	m_srvDescriptor = descManager->AllocDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    // Describe and create a Texture2D.
    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels = 1;
    textureDesc.Format = m_format;
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.Flags = flag;
    textureDesc.DepthOrArraySize = 1;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

    auto hp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(device->CreateCommittedResource(
        &hp,
        D3D12_HEAP_FLAG_NONE,
        &textureDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&m_buffer)));
}

void Texture2D::LoadFromData(int w, int h, DXGI_FORMAT fmt, void* pData, UINT pixelByteSize)
{
    GraphicContex* contex = GraphicContex::GetInstance();
    ID3D12Device* device = contex->GetDevice();
    ID3D12GraphicsCommandList* cmdList = contex->GetCommandList();

    CreateEmpty(w, h, fmt);

    // warning : for texture2D, buffer size always bigger than 1D buffer
    // using [size = width * height * 4] will cause some problems ...
    const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_buffer.Get(), 0, 1);

    // Create the GPU upload buffer.
    ComPtr<ID3D12Resource> textureUploadHeap;
    auto uhp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto ubf = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
    ThrowIfFailed(device->CreateCommittedResource(
        &uhp,
        D3D12_HEAP_FLAG_NONE,
        &ubf,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&textureUploadHeap)));

    // Copy data to the intermediate upload heap and then schedule a copy from the upload heap to the Texture2D.
    D3D12_SUBRESOURCE_DATA textureData = {};
    textureData.pData = pData;
    textureData.RowPitch = width * pixelByteSize;
    textureData.SlicePitch = textureData.RowPitch * height;

    UpdateSubresources(cmdList, m_buffer.Get(), textureUploadHeap.Get(), 0, 0, 1, &textureData);

    // ba
    auto trans = CD3DX12_RESOURCE_BARRIER::Transition(m_buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    cmdList->ResourceBarrier(1, &trans);

    // Describe and create a SRV for the texture.
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = fmt;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    device->CreateShaderResourceView(m_buffer.Get(), &srvDesc, m_srvDescriptor.cpuHandle);

    // upload to gpu
    contex->SyncExecute(cmdList);
}

void Texture2D::LoadFromFile(std::string filepath)
{
    name = filepath;
    std::string extname = filepath.substr(filepath.find_last_of(".") + 1);

    if (extname == "jpg" || extname == "png")
    {
        // read data from file
        int nChannels , w, h;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(filepath.c_str(), &w, &h, &nChannels, 4);

        LoadFromData(w, h, DXGI_FORMAT_R8G8B8A8_UNORM, data, 4);
    }

    if (extname == "hdr")
    {

    }
}

CD3DX12_GPU_DESCRIPTOR_HANDLE Texture2D::GetGpuHandle()
{
    return m_srvDescriptor.gpuHandle;
}
