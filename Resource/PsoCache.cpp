#include "PsoCache.h"

#include "../Core/GraphicContex.h"
#include "../Core/helper.h"

#include "../Resource/ResourceLoader.hpp"

PsoCache::PsoCache()
{

}

PsoCache::~PsoCache()
{

}

PsoCache* PsoCache::GetInstance()
{
	static PsoCache instance;
	return &instance;
}

ID3D12PipelineState* PsoCache::Find(const PsoDescriptor& pd)
{
	auto it = m_psoPool.find(pd);
	if (!(it == m_psoPool.end()))
	{
		return it->second.Get();
	}

    // Define the vertex input layout.
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // find shader by name
    auto pShader = ResourceLoader<Shader>::GetInstance()->Find(pd.shaderRef);

    // Describe and create the graphics pipeline state object (PSO).
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = pShader->m_rootSignature.Get();
    psoDesc.VS = CD3DX12_SHADER_BYTECODE(pShader->m_vertexShaderByteCode.Get());
    psoDesc.PS = CD3DX12_SHADER_BYTECODE(pShader->m_pixelShaderByteCode.Get());

    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.SampleDesc.Count = 1;

    if (pd.blendMode == EBlendMode::Opaque)
    {
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    }
    
    psoDesc.NumRenderTargets = pd.NumRenderTargets;
    for (int i = 0; i < pd.NumRenderTargets; i++)
    {
        psoDesc.RTVFormats[i] = pd.RTVFormats[i];
    }
    
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // a default depth stencil state
    psoDesc.DSVFormat = pd.DSVFormat;

    auto device = GraphicContex::GetInstance()->GetDevice();
    ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_psoPool[pd])));

	return m_psoPool[pd].Get();
}