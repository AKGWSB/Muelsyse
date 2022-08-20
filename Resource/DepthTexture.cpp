#include "DepthTexture.h"

#include "../Core/GraphicContex.h"
#include "../Core/helper.h"

DepthTexture::DepthTexture(int w, int h)
{
    DescriptorManager* descManager = DescriptorManager::GetInstance();
    GraphicContex* contex = GraphicContex::GetInstance();
    ID3D12Device* device = contex->GetDevice();

    width = w;
    height = h;
    m_format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    // Describe and create a Texture2D.
    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels = 1;
    textureDesc.Format = m_format;
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    textureDesc.DepthOrArraySize = 1;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

    D3D12_CLEAR_VALUE clearValue = {};
    clearValue.Format = textureDesc.Format;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    auto hp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(device->CreateCommittedResource(
        &hp,
        D3D12_HEAP_FLAG_NONE,
        &textureDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue,
        IID_PPV_ARGS(&m_buffer)));

    // alloc dsv. srv desc
    m_dsvDescriptor = descManager->AllocDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    m_srvDescriptor = descManager->AllocDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    // Describe and create a DSV for the texture.
    D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;
    device->CreateDepthStencilView(m_buffer.Get(), &depthStencilDesc, m_dsvDescriptor.cpuHandle);

    // Describe and create a SRV for the texture.
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; // for depth tex's srv
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    device->CreateShaderResourceView(m_buffer.Get(), &srvDesc, m_srvDescriptor.cpuHandle);
    
}

DepthTexture::~DepthTexture()
{
    DescriptorManager* descManager = DescriptorManager::GetInstance();

    descManager->FreeDescriptor(m_dsvDescriptor);
    descManager->FreeDescriptor(m_srvDescriptor);
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DepthTexture::GetDsvCpuHandle()
{
    return m_dsvDescriptor.cpuHandle;
}