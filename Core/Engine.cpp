#include <memory>

#include "helper.h"
#include "Win32App.h"

#include "Engine.h"
#include "GraphicContex.h"

#include "../Resource/Mesh.h"
#include "../Resource/Shader.h"
#include "../Resource/Texture2D.h"
#include "../Resource/RenderTexture.h"
#include "../Resource/Material.h"
#include "../Resource/ResourceLoader.hpp"

#include "../Rendering/Transform.h"

using Microsoft::WRL::ComPtr;

// ---------------------------------------------------------------------- //

ComPtr<ID3D12PipelineState> m_pipelineState;

Shader* m_shader;
std::unique_ptr<Mesh> m_mesh;
std::unique_ptr<Material> m_material;
std::unique_ptr<RenderTexture> m_rt0;

std::unique_ptr<UploadBuffer> m_cb0;
std::unique_ptr<UploadBuffer> m_cb1;

Transform transform;
Transform transformInv;

// ---------------------------------------------------------------------- //

Engine::Engine()
{

}

Engine::~Engine()
{

}

void Engine::OnInit()
{
    GraphicContex::GetInstance()->Init();

    ResourceLoader<Texture2D>* texLoader = ResourceLoader<Texture2D>::GetInstance();
    ResourceLoader<Shader>* shaderLoader = ResourceLoader<Shader>::GetInstance();

    m_cb0 = std::make_unique<UploadBuffer>();
    m_cb1 = std::make_unique<UploadBuffer>();

    m_mesh = std::make_unique<Mesh>();
    m_shader = shaderLoader->Find("Shaders/test.hlsl");
    m_rt0 = std::make_unique<RenderTexture>(1600, 900, DXGI_FORMAT_R8G8B8A8_UNORM);

    m_material = std::make_unique<Material>();
    m_material->SetShader(m_shader);
    m_material->SetTexture("mainTex0", texLoader->Find("Asset/test2.jpg"));
    //m_material->SetTexture("mainTex1", texLoader->Find("Asset/test3.jpg"));

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
    psoDesc.pRootSignature = m_shader->m_rootSignature.Get();
    psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_shader->m_vertexShaderByteCode.Get());
    psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_shader->m_pixelShaderByteCode.Get());

    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;

    auto device = GraphicContex::GetInstance()->GetDevice();
    ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
}

void Engine::OnUpdate()
{
    double delta_time = m_timer.End();
    Tick(delta_time);
    m_timer.Start();
}

void Engine::OnDestroy()
{
    delete m_material.release();
    delete m_rt0.release();
    delete m_cb0.release();
    delete m_cb1.release();

    ResourceLoader<Texture2D>::GetInstance()->Shutdown();
    ResourceLoader<Shader>::GetInstance()->Shutdown();
}

// ---------------------------------------------------------------------- //

void Engine::OnRender()
{
    auto contex = GraphicContex::GetInstance();
    auto cmdList = contex->GetCommandList();
    
    contex->Begin();

    {
        cmdList->SetPipelineState(m_pipelineState.Get());

        // first pass
        /**/
        {
            contex->SetRenderTarget(cmdList, { m_rt0.get() });
            contex->ClearRenderTarget(cmdList, m_rt0.get(), Vector3(0.0, 0.0, 0.0));
            contex->SetViewPort(cmdList, Vector4(0, 0, Win32App::m_width, Win32App::m_height));

            m_material->SetTexture("mainTex0", ResourceLoader<Texture2D>::GetInstance()->Find("Asset/test2.jpg"));
            m_material->SetCbuffer("cbPreObject", m_cb0.get());
            m_material->SetMatrix("cbPreObject", "modelMatrix", transform.GetTransformMatrix());
            m_material->Activate(cmdList);
            m_mesh->Draw(cmdList);

            m_rt0->ChangeToShaderRsourceState(cmdList);
        }
        
        // second pass
        {
            contex->SetRenderTarget(cmdList);   // set to screen
            contex->ClearRenderTarget(cmdList, Vector3(0.5, 0.5, 0.5));
            contex->SetViewPort(cmdList, Vector4(0, 0, Win32App::m_width, Win32App::m_height));

            m_material->SetTexture("mainTex0", m_rt0.get());
            m_material->SetCbuffer("cbPreObject", m_cb1.get());
            m_material->SetMatrix("cbPreObject", "modelMatrix", transformInv.GetTransformMatrix());
            m_material->Activate(cmdList);
            m_mesh->Draw(cmdList);
        }
    }
    
    contex->End();
}

// ---------------------------------------------------------------------- //

void Engine::Tick(double delta_time)
{
    transform.rotation.z += 45.0f * delta_time;
    transformInv.rotation.z += 45.0f * delta_time;
}