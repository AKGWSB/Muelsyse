#include <memory>

#include "Engine.h"
#include "helper.h"
#include "GraphicContex.h"
#include "Win32App.h"

#include "../Resource/Mesh.h"
#include "../Resource/Shader.h"
#include "../Resource/Texture2D.h"
#include "../Rendering/Transform.h"

using Microsoft::WRL::ComPtr;

// ---------------------------------------------------------------------- //

std::unique_ptr<Mesh> m_mesh;
std::unique_ptr<Shader> m_shader;
std::unique_ptr<Texture2D> m_texture0;
std::unique_ptr<Texture2D> m_texture1;
ComPtr<ID3D12PipelineState> m_pipelineState;
std::unique_ptr<UploadBuffer> m_cbuffer;
Transform transform;

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

    m_mesh = std::make_unique<Mesh>();
    m_shader = std::make_unique<Shader>("Shaders/test.hlsl");
    m_texture0 = std::make_unique<Texture2D>("Asset/test2.jpg");
    m_texture1 = std::make_unique<Texture2D>("Asset/test3.jpg");
    m_cbuffer = std::make_unique<UploadBuffer>();
   
    m_shader->SetCbuffer("cbPreObject", m_cbuffer.get());
    m_shader->SetTexture("mainTex0", m_texture0.get());
    m_shader->SetTexture("mainTex1", m_texture1.get());

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
    delete m_texture0.release();
    delete m_texture1.release();
    delete m_cbuffer.release();
}

// ---------------------------------------------------------------------- //

void Engine::OnRender()
{
    auto contex = GraphicContex::GetInstance();
    auto cmdList = contex->GetCommandList();
    
    contex->Begin();

    {
        contex->SetRenderTarget();
        contex->ClearRenderTarget(Vector3(0.5, 0.5, 0.5));
        contex->SetViewPort(Vector4(0, 0, Win32App::m_width, Win32App::m_height));

        cmdList->SetPipelineState(m_pipelineState.Get());

        m_shader->SetMatrix("cbPreObject", "modelMatrix", transform.GetTransformMatrix());
        m_shader->Activate();
        m_mesh->Draw();
    }
    
    contex->End();
}

// ---------------------------------------------------------------------- //

void Engine::Tick(double delta_time)
{
    transform.rotation.z += 45.0f * delta_time;
}