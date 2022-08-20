#include <memory>

#include "helper.h"
#include "Win32App.h"

#include "Engine.h"
#include "GraphicContex.h"
#include "CommandListHandle.h"

#include "../Resource/Mesh.h"
#include "../Resource/Shader.h"
#include "../Resource/Texture2D.h"
#include "../Resource/RenderTexture.h"
#include "../Resource/DepthTexture.h"
#include "../Resource/Material.h"
#include "../Resource/ResourceLoader.hpp"

#include "../Rendering/Transform.h"
#include "../Rendering/PsoCache.h"

#include "../Gameplay/StaticMeshComponent.h"
#include "../Gameplay/Actor.h"

using Microsoft::WRL::ComPtr;

// ---------------------------------------------------------------------- //

std::unique_ptr<RenderTexture> m_rt0;

Shader* m_shader;
std::unique_ptr<Material> m_cubeMaterial;
std::unique_ptr<StaticMeshComponent> m_cubeMeshComp;

std::unique_ptr<Material> m_sphereMaterial;
std::unique_ptr<StaticMeshComponent> m_sphereMeshComp;

std::unique_ptr<Actor> m_actor;

std::unique_ptr<DepthTexture> m_depthTex;

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
    ResourceLoader<Mesh>* meshLoader = ResourceLoader<Mesh>::GetInstance();

    m_rt0 = std::make_unique<RenderTexture>(Win32App::m_width, Win32App::m_height, DXGI_FORMAT_R8G8B8A8_UNORM);
    m_depthTex = std::make_unique<DepthTexture>(Win32App::m_width, Win32App::m_height);

    m_shader = shaderLoader->Find("Shaders/test.hlsl");

    // cube
    {
        m_cubeMaterial = std::make_unique<Material>();
        m_cubeMaterial->SetShader(m_shader);
        m_cubeMaterial->SetTexture("mainTex0", texLoader->Find("Asset/test2.jpg"));

        m_cubeMeshComp = std::make_unique<StaticMeshComponent>();
        m_cubeMeshComp->m_material = m_cubeMaterial.get();
        m_cubeMeshComp->m_mesh = meshLoader->Find("Asset/Geometry/cube.obj");
        m_cubeMeshComp->m_transform.translate = Vector3(-0.35, 0, 0.0);
        m_cubeMeshComp->m_transform.scale = Vector3(0.25, 0.25, 0.25);
    }
       
    // sphere
    {
        m_sphereMaterial = std::make_unique<Material>();
        m_sphereMaterial->SetShader(m_shader);
        m_sphereMaterial->SetTexture("mainTex0", texLoader->Find("Asset/test3.jpg"));

        m_sphereMeshComp = std::make_unique<StaticMeshComponent>();
        m_sphereMeshComp->m_material = m_sphereMaterial.get();
        m_sphereMeshComp->m_mesh = meshLoader->Find("Asset/Geometry/sphere.obj");
        m_sphereMeshComp->m_transform.translate = Vector3(0.35, 0, 0.0);
        m_sphereMeshComp->m_transform.scale = Vector3(0.25, 0.25, 0.25);
    }

    // pack to an actor
    {
        m_actor = std::make_unique<Actor>();
        m_actor->transform.translate = Vector3(0, 0, 0.5);
        m_actor->RegisterComponent("CbueMesh", m_cubeMeshComp.get());
        m_actor->RegisterComponent("SphereMesh", m_sphereMeshComp.get());
    }
}

void Engine::OnUpdate()
{
    double delta_time = m_timer.End();
    Tick(delta_time);
    m_timer.Start();
}

void Engine::OnDestroy()
{
    delete m_cubeMaterial.release();
    delete m_cubeMeshComp.release();
    delete m_sphereMeshComp.release();
    delete m_rt0.release();
    delete m_depthTex.release();

    ResourceLoader<Texture2D>::GetInstance()->Shutdown();
    ResourceLoader<Shader>::GetInstance()->Shutdown();
    ResourceLoader<Mesh>::GetInstance()->Shutdown();

    GraphicContex::GetInstance()->Shutdown();
}

// ---------------------------------------------------------------------- //

void Engine::OnRender()
{
    auto contex = GraphicContex::GetInstance();
    auto cmdList = contex->GetCommandList();
    
    contex->Begin();

    {
        
        PsoDescriptor psoDesc;
        psoDesc.shaderRef = "Shaders/test.hlsl";

        auto psoCache = PsoCache::GetInstance();
        ID3D12PipelineState* m_pipelineState = psoCache->Find(psoDesc);
        cmdList->SetPipelineState(m_pipelineState);

        CommandListHandle* cmd = new CommandListHandle(cmdList);

        // first pass
        {
            RenderTexture* screen = contex->GetCurrentBackBuffer();
            
            cmd->SetRenderTarget({ screen }, m_depthTex.get());
            cmd->ClearRenderTarget({ screen }, Vector3(0.5, 0.5, 0.5));
            cmd->ClearDepthBuffer(m_depthTex.get());
            cmd->SetViewPort(Vector4(0, 0, Win32App::m_width, Win32App::m_height));

            cmd->RenderActor(m_actor.get());
        }

        delete cmd;
    }
    
    contex->End();
}

// ---------------------------------------------------------------------- //

void Engine::Tick(double delta_time)
{
    m_actor->transform.rotation.y += 90.0f * delta_time;
}