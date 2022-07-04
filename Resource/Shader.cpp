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
        ThrowIfFailed(D3DCompileFromFile(l_filepath, nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, vertexShaderByteCode.GetAddressOf(), &vsErrMsg));
    }
    catch (std::exception e)
    {
        OutputDebugStringA((char*)vsErrMsg->GetBufferPointer());
        throw std::exception((char*)vsErrMsg->GetBufferPointer());
    }
        
    try
    {
        ThrowIfFailed(D3DCompileFromFile(l_filepath, nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, pixelShaderByteCode.GetAddressOf(), &psErrMsg));

    }
    catch (std::exception e)
    {
        OutputDebugStringA((char*)psErrMsg->GetBufferPointer());
        throw std::exception((char*)psErrMsg->GetBufferPointer());
    }

    // var refleciton and generate parameter signature
    RecordReflectionInfo();
    CreateRootSignature();
}

void Shader::RecordReflectionInfo()
{
    auto shaderByteCodes = { vertexShaderByteCode, pixelShaderByteCode };

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
                textureBindInfo[resourceName].bindRegister = bindRegister;
                textureBindInfo[resourceName].bindRegisterSpace = bindRegisterSpace;
            }

            // cbuffer
            if (resourceType == D3D_SIT_CBUFFER)
            {
                // record bind point
                cbufferBindInfo[resourceName].bindRegister = bindRegister;
                cbufferBindInfo[resourceName].bindRegisterSpace = bindRegisterSpace;

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

                    cbufferBindInfo[resourceName].variables[varName].offset = varOffset;
                    cbufferBindInfo[resourceName].variables[varName].size = varSize;

                    // OutputDebugStringA("for debug zz");
                }
            }
        }
    }
}

void Shader::CreateRootSignature()
{
    int paramNum = cbufferBindInfo.size() + textureBindInfo.size();
    int i = 0;
    std::vector<CD3DX12_DESCRIPTOR_RANGE> ranges(paramNum);
    
    // first handle cbuffer
    for (auto& p : cbufferBindInfo)
    {
        auto& name = p.first;
        auto& info = p.second;

        info.rootParameterIndex = i;
        ranges[i].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, info.bindRegister, info.bindRegisterSpace);
        i++;
    }

    // then handle texture
    for(auto& p : textureBindInfo)
    {
        auto& name = p.first;
        auto& info = p.second;

        info.rootParameterIndex = i;
        ranges[i].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, info.bindRegister, info.bindRegisterSpace);
        i++;
    }

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

    ThrowIfFailed(GraphicContex::g_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));

}

// bind descriptor handle 
void Shader::Activate()
{
    // set signature
    GraphicContex::g_commandList->SetGraphicsRootSignature(rootSignature.Get());

    // bind resource

    for (auto& p : cbufferBindInfo)
    {
        auto& name = p.first;
        auto& info = p.second;
        if (info.cbuffer == NULL) continue;

        // upload cbuffer data
        for (auto& v : info.variables)
        {
            auto& var = v.second;

            // copy from var to cbuffer cpu buffer
            info.cbuffer->UpdateSubData(var.offset, var.size, var.data);
        }
        info.cbuffer->Upload();     // copy from cpu to gpu

        GraphicContex::g_commandList->SetGraphicsRootDescriptorTable(info.rootParameterIndex, info.cbuffer->cbvGpuHandle);
    }

    for (auto& p : textureBindInfo)
    {
        auto& name = p.first;
        auto& info = p.second;
        if (info.texture == NULL) continue;

        GraphicContex::g_commandList->SetGraphicsRootDescriptorTable(info.rootParameterIndex, info.texture->srvGpuHandle);
    }
}

void Shader::DeActivateForDebug()
{
    Texture2D* tex404 = Texture2D::Find("Core/TEXTURE_NOT_FOUND.png");

    for (auto& p : textureBindInfo)
    {
        auto& info = p.second;
        info.texture = tex404;
    }
}

void Shader::SetTexture(std::string textureName, Texture2D* src)
{
    if (textureBindInfo.find(textureName) == textureBindInfo.end()) return;

    textureBindInfo[textureName].texture = src;
}

void Shader::SetCbuffer(std::string bufferName, UploadBuffer* src)
{
    if (cbufferBindInfo.find(bufferName) == cbufferBindInfo.end()) return;

    cbufferBindInfo[bufferName].cbuffer = src;
}

void Shader::SetMatrix(std::string bufferName, std::string varName, XMMATRIX src)
{
    if (cbufferBindInfo.find(bufferName) == cbufferBindInfo.end()) return;
    if (cbufferBindInfo[bufferName].variables.find(varName) == cbufferBindInfo[bufferName].variables.end()) return;

    void* dst = cbufferBindInfo[bufferName].variables[varName].data;
    memcpy(dst, &src, sizeof(XMMATRIX));
}

void Shader::SetFloat4(std::string bufferName, std::string varName, XMFLOAT4 src)
{
    if (cbufferBindInfo.find(bufferName) == cbufferBindInfo.end()) return;
    if (cbufferBindInfo[bufferName].variables.find(varName) == cbufferBindInfo[bufferName].variables.end()) return;

    void* dst = cbufferBindInfo[bufferName].variables[varName].data;
    memcpy(dst, &src, sizeof(XMFLOAT4));
}


// global resource pool, find by filename
std::map<std::string, std::unique_ptr<Shader>> Shader::g_shaderResourceMap;
Shader* Shader::Find(std::string filepath)
{
    if (g_shaderResourceMap.find(filepath) == g_shaderResourceMap.end())
    {
        g_shaderResourceMap[filepath] = std::make_unique<Shader>(filepath);
    }
    return g_shaderResourceMap[filepath].get();
}

void Shader::FreeAll()
{
    for (auto& p : g_shaderResourceMap)
    {
        delete p.second.release();
    }
}