#include "Shader.h"
#include "../Core/d3dx12.h"
#include "../Core/helper.h"
#include "../Core/GraphicContex.h"

#include <D3Dcompiler.h>

Shader::Shader(std::string filepath)
{
    LoadFromFile(filepath);
    name = filepath;
}

Shader::~Shader()
{   

}

void Shader::LoadFromFile(std::string filepath)
{
    LPCWSTR l_filepath = STRING_TO_LPCWSTR(filepath);

#if defined(_DEBUG)
    // Enable better shader debugging with the graphics debugging tools.
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif
    ComPtr<ID3DBlob> vsErrMsg;
    ComPtr<ID3DBlob> psErrMsg;

    try
    {
        ThrowIfFailed(D3DCompileFromFile(l_filepath, nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, m_vertexShaderByteCode.GetAddressOf(), &vsErrMsg));
    }
    catch (std::exception e)
    {
        OutputDebugStringA((char*)vsErrMsg->GetBufferPointer());
        throw std::exception((char*)vsErrMsg->GetBufferPointer());
    }
        
    try
    {
        ThrowIfFailed(D3DCompileFromFile(l_filepath, nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, m_pixelShaderByteCode.GetAddressOf(), &psErrMsg));

    }
    catch (std::exception e)
    {
        OutputDebugStringA((char*)psErrMsg->GetBufferPointer());
        throw std::exception((char*)psErrMsg->GetBufferPointer());
    }

    CreateRootSignature();
}

void Shader::CreateRootSignature()
{
    auto shaderByteCodes = { m_vertexShaderByteCode, m_pixelShaderByteCode };

    // shader reflection, both vs and ps
    for (auto& shaderByteCode : shaderByteCodes)
    {
        ID3D12ShaderReflection* reflection = NULL;
        D3DReflect(shaderByteCode->GetBufferPointer(), shaderByteCode->GetBufferSize(), IID_ID3D12ShaderReflection, (void**)&reflection);

        D3D12_SHADER_DESC shaderDesc;
        reflection->GetDesc(&shaderDesc);

        for (UINT i = 0; i < shaderDesc.BoundResources; i++)
        {
            D3D12_SHADER_INPUT_BIND_DESC  resourceDesc;
            reflection->GetResourceBindingDesc(i, &resourceDesc);

            auto resourceName = resourceDesc.Name;
            auto resourceType = resourceDesc.Type;
            auto bindRegisterSpace = resourceDesc.Space;
            auto bindRegister = resourceDesc.BindPoint;
            auto bindCount = resourceDesc.BindCount;

            // ignore sampler
            if (resourceType == D3D_SIT_SAMPLER)
            {
                continue;
            }

            // texture
            if (resourceType == D3D_SIT_TEXTURE)
            {
                // record bind point
                m_textureBindInfoMap[resourceName].bindRegister = bindRegister;
                m_textureBindInfoMap[resourceName].bindRegisterSpace = bindRegisterSpace;
            }

            // cbuffer
            if (resourceType == D3D_SIT_CBUFFER)
            {
                // record bind point
                m_cbufferBindInfoMap[resourceName].bindRegister = bindRegister;
                m_cbufferBindInfoMap[resourceName].bindRegisterSpace = bindRegisterSpace;

                // record cbuffer memory layout
                auto cbuffer = reflection->GetConstantBufferByName(resourceName);
                D3D12_SHADER_BUFFER_DESC cbufferDesc;
                cbuffer->GetDesc(&cbufferDesc);

                // loop and config cbuffer's variable
                for (UINT j = 0; j < cbufferDesc.Variables; j++)
                {
                    auto cbufferVar = cbuffer->GetVariableByIndex(j);

                    D3D12_SHADER_VARIABLE_DESC varDesc;
                    cbufferVar->GetDesc(&varDesc);

                    auto varName = varDesc.Name;
                    auto varOffset = varDesc.StartOffset;
                    auto varSize = varDesc.Size;

                    m_cbufferBindInfoMap[resourceName].variableDescMap[varName].offset = varOffset;
                    m_cbufferBindInfoMap[resourceName].variableDescMap[varName].size = varSize;
                }
            }
        }
    }

    int paramNum = m_cbufferBindInfoMap.size() + m_textureBindInfoMap.size();
    int i = 0;
    std::vector<CD3DX12_DESCRIPTOR_RANGE> ranges(paramNum);

    // first handle cbuffer
    for (auto& p : m_cbufferBindInfoMap)
    {
        auto& name = p.first;
        auto& info = p.second;

        info.rootParameterIndex = i;
        ranges[i].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, info.bindRegister, info.bindRegisterSpace);
        i++;
    }

    // then handle texture
    for (auto& p : m_textureBindInfoMap)
    {
        auto& name = p.first;
        auto& info = p.second;

        info.rootParameterIndex = i;
        ranges[i].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, info.bindRegister, info.bindRegisterSpace);
        i++;
    }

    GraphicContex* contex = GraphicContex::GetInstance();
    ID3D12Device* device = contex->GetDevice();

    // setup root parameter
    // ranges[i] is a descriptor table with length=1
    // range[i] bind to root parameter[i]
    std::vector<CD3DX12_ROOT_PARAMETER> rootParameters(paramNum);
    for (int i = 0; i < ranges.size(); i++)
    {
        rootParameters[i].InitAsDescriptorTable(1, &ranges[i]);
    }

    // default sampler
    D3D12_STATIC_SAMPLER_DESC sampler = {};
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
    sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    sampler.MipLODBias = 0;
    sampler.MaxAnisotropy = 0;
    sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    sampler.MinLOD = 0.0f;
    sampler.MaxLOD = D3D12_FLOAT32_MAX;
    sampler.ShaderRegister = 0;
    sampler.RegisterSpace = 0;
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    // Create a root signature.
    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init(rootParameters.size(), rootParameters.data(), 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    try
    {
        ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
    }
    catch (std::exception e)
    {
        OutputDebugStringA((char*)error->GetBufferPointer());
        throw std::exception((char*)error->GetBufferPointer());
    }

    ThrowIfFailed(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));

}

// bind descriptor handle 
void Shader::Activate()
{
    ID3D12GraphicsCommandList* cmdList = GraphicContex::GetInstance()->GetCommandList();

    // set signature
    cmdList->SetGraphicsRootSignature(m_rootSignature.Get());

    // bind resource
    for (auto& p : m_cbufferBindInfoMap)
    {
        auto& name = p.first;
        auto& info = p.second;

        if (info.cbuffer == NULL) continue;
        info.cbuffer->Upload();     // copy from cpu to gpu

        cmdList->SetGraphicsRootDescriptorTable(info.rootParameterIndex, info.cbuffer->GetGpuHandle());
    }

    for (auto& p : m_textureBindInfoMap)
    {
        auto& name = p.first;
        auto& info = p.second;
        if (info.texture == NULL) continue;

        cmdList->SetGraphicsRootDescriptorTable(info.rootParameterIndex, info.texture->GetGpuHandle());
    }
}


void Shader::SetTexture(std::string textureName, Texture2D* src)
{
    if (m_textureBindInfoMap.find(textureName) == m_textureBindInfoMap.end()) return;

    m_textureBindInfoMap[textureName].texture = src;
}

void Shader::SetCbuffer(std::string bufferName, UploadBuffer* src)
{
    if (m_cbufferBindInfoMap.find(bufferName) == m_cbufferBindInfoMap.end()) return;

    m_cbufferBindInfoMap[bufferName].cbuffer = src;
}

void Shader::SetMatrix(std::string bufferName, std::string varName, Matrix src)
{
    if (m_cbufferBindInfoMap.find(bufferName) == m_cbufferBindInfoMap.end()) return;
    if (m_cbufferBindInfoMap[bufferName].variableDescMap.find(varName) == m_cbufferBindInfoMap[bufferName].variableDescMap.end()) return;

    auto resourceDesc = m_cbufferBindInfoMap[bufferName];
    auto varDesc = resourceDesc.variableDescMap[varName];
    UINT offset = varDesc.offset;
    UINT size = varDesc.size;
    
    resourceDesc.cbuffer->UpdateSubData(offset, size, &src);
}

void Shader::SetFloat4(std::string bufferName, std::string varName, Vector4 src)
{
    if (m_cbufferBindInfoMap.find(bufferName) == m_cbufferBindInfoMap.end()) return;
    if (m_cbufferBindInfoMap[bufferName].variableDescMap.find(varName) == m_cbufferBindInfoMap[bufferName].variableDescMap.end()) return;

    auto resourceDesc = m_cbufferBindInfoMap[bufferName];
    auto varDesc = resourceDesc.variableDescMap[varName];
    UINT offset = varDesc.offset;
    UINT size = varDesc.size;

    resourceDesc.cbuffer->UpdateSubData(offset, size, &src);
}
