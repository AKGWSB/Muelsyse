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
#include "../Resource/PsoCache.h"

#include "../Rendering/Transform.h"
#include "../Rendering/RenderQueue.h"
#include "../Rendering/Renderer.h"
#include "../Rendering/MeshPass.h"
#include "../Rendering/ScreenPass.h"

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

std::unique_ptr<Material> m_planeMaterial;
std::unique_ptr<StaticMeshComponent> m_planeMeshComp;

std::unique_ptr<Actor> m_actor0;
std::unique_ptr<Actor> m_actor1;

std::unique_ptr<DepthTexture> m_depthTex;

std::unique_ptr<MeshPass> m_basePass;
std::unique_ptr<ScreenPass> m_blitPass;

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

    // plane
    {
        m_planeMaterial = std::make_unique<Material>();
        m_planeMaterial->SetShader(m_shader);
        m_planeMaterial->SetTexture("mainTex0", texLoader->Find("Asset/spaceship/StarSparrow_Red.png"));

        m_planeMeshComp = std::make_unique<StaticMeshComponent>();
        m_planeMeshComp->m_material = m_planeMaterial.get();
        m_planeMeshComp->m_mesh = meshLoader->Find("Asset/spaceship/StarSparrow01.obj");
        m_planeMeshComp->m_transform.translate = Vector3(0, 0, 0.0);
    }

    // pack comp to actor
    {
        m_actor0 = std::make_unique<Actor>();
        m_actor0->transform.translate = Vector3(0, 0, 0.25);
        m_actor0->RegisterComponent("CbueMesh", m_cubeMeshComp.get());
        m_actor0->RegisterComponent("SphereMesh", m_sphereMeshComp.get());

        m_actor1 = std::make_unique<Actor>();
        m_actor1->transform.translate = Vector3(0, -0.35, 0.0);
        m_actor1->RegisterComponent("MeshComp", m_planeMeshComp.get());
    }

    m_basePass = std::make_unique<MeshPass>();
    m_basePass->renderTargets.push_back(m_rt0.get());
    m_basePass->depthTex = m_depthTex.get();

    m_blitPass = std::make_unique<ScreenPass>();
    m_blitPass->shader = ResourceLoader<Shader>::GetInstance()->Find("Shaders/blit_test.hlsl");
    m_blitPass->renderTargets.push_back(GraphicContex::GetInstance()->GetCurrentBackBuffer());
    m_blitPass->texturesInputPrePass["mainTex0"] = m_rt0.get();
    m_blitPass->texturesInputPrePass["mainTex1"] = m_depthTex.get();
    m_blitPass->depthTex = NULL;

    Camera* mainCam = Camera::GetMain();
    mainCam->aspect = float(Win32App::m_width) / float(Win32App::m_height);
    mainCam->trnasform.translate = Vector3(0.0, 1, -2);
    mainCam->trnasform.rotation = Vector3(-30, 0, 0);
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

    delete m_sphereMaterial.release();
    delete m_sphereMeshComp.release();

    delete m_planeMaterial.release();
    delete m_planeMeshComp.release();
   
    delete m_rt0.release();
    delete m_depthTex.release();
    delete m_basePass.release();

    ResourceLoader<Texture2D>::GetInstance()->Shutdown();
    ResourceLoader<Shader>::GetInstance()->Shutdown();
    ResourceLoader<Mesh>::GetInstance()->Shutdown();

    GraphicContex::GetInstance()->Shutdown();
}

// ---------------------------------------------------------------------- //

void Engine::OnRender()
{
    // populate render queue
    {
        m_actor0->OnRender();
        m_actor1->OnRender();
    }

    auto contex = GraphicContex::GetInstance();
    auto cmdList = contex->GetCommandList();

    // draw
    contex->Begin();
    {
        CommandListHandle* cmd = new CommandListHandle(cmdList);

        // first pass
        m_basePass->renderTargets[0] = m_rt0.get();
        m_basePass->Forward(cmd, Camera::GetMain());

        // blit pass
        m_blitPass->renderTargets[0] = contex->GetCurrentBackBuffer();
        m_blitPass->Forward(cmd);

        delete cmd;
    }
    contex->End();
}

// ---------------------------------------------------------------------- //

void Engine::Tick(double delta_time)
{
    m_actor0->transform.rotation.y += 190.0f * delta_time;
    m_actor1->transform.rotation.y -= 45.0f * delta_time;
}