#include "RenderPass.h"
#include "../Core/helper.h"
#include "../Core/GraphicContex.h"

RenderPass::RenderPass()
{
    cbufferPrePass = std::make_unique<UploadBuffer>();
}

RenderPass::~RenderPass()
{

}

void RenderPass::InitPipelineState(Shader* shader)
{
    // Define the vertex input layout.
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // Describe and create the graphics pipeline state object (PSO).
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    psoDesc.pRootSignature = shader->rootSignature.Get();
    psoDesc.VS = CD3DX12_SHADER_BYTECODE(shader->vertexShaderByteCode.Get());
    psoDesc.PS = CD3DX12_SHADER_BYTECODE(shader->pixelShaderByteCode.Get());
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    // default ban depth
    if (depthTex == NULL)
    {
        psoDesc.DepthStencilState.DepthEnable = FALSE;
    }
    // if has been set depth component
    else 
    {
        psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        psoDesc.DepthStencilState.DepthEnable = TRUE;
        psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    }
    
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    // default render to screen
    if (renderTargets.size() == 0)
    {
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    }
    // custom MRT
    else
    {
        psoDesc.NumRenderTargets = renderTargets.size();
        for (int i = 0; i < renderTargets.size(); i++)
        {
            psoDesc.RTVFormats[i] = renderTargets[i]->format;
        }
    }
        
    psoDesc.SampleDesc.Count = 1;

    auto shaderName = shader->name;
    ThrowIfFailed(GraphicContex::g_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineStatePool[shaderName])));
}

ID3D12PipelineState* RenderPass::GetPsoByShader(Shader* shader)
{
    auto shaderName = shader->name;

    // if not exist, create PSO
    if (pipelineStatePool.find(shaderName) == pipelineStatePool.end())
    {
        InitPipelineState(shader);
    }

    return pipelineStatePool[shaderName].Get();
}