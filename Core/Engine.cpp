#include "Engine.h"
#include "GraphicContex.h"
#include "helper.h"

#include "../Rendering/Transform.h"
#include "../Rendering/Camera.h"
#include "../Rendering/RenderPass.h"
#include "../Rendering/RenderTexture.h"
#include "../Rendering/DepthTexture.h"

#include "../Editor/Actor.h"
#include "../Editor/Editor.h"

// App resources.
Camera* camera;

RenderTexture* RT_basePass;
RenderTexture* RT_final;
DepthTexture* RT_basePassDepth;

std::unique_ptr<RenderPass> basePass;
std::unique_ptr<RenderPass> finalPass;

std::unique_ptr<Actor> A_quad;

std::unique_ptr<Editor> editor;
std::unique_ptr<Scene> scene;

Engine::Engine()
{

}

Engine::~Engine()
{

}

void Engine::OnInit()
{
	GraphicContex::Init(g_width, g_height, g_hwnd);

    // 
    camera = new Camera(g_width, g_height);
    camera->SetPosition(XMFLOAT3(0, 1, -4));
    camera->SetTarget(XMFLOAT3(0, 0, 0));

    // create rt
    RenderTexture* RT_basePass = RenderTexture::Create("RT_basePass", g_width, g_height, DXGI_FORMAT_R32G32B32A32_FLOAT);
    RenderTexture* RT_final = RenderTexture::Create("RT_final", g_width, g_height, DXGI_FORMAT_R8G8B8A8_UNORM);
    DepthTexture* RT_basePassDepth = DepthTexture::Create("DT_basePass", g_width, g_height);

    // create custom pass
    {
        basePass = std::make_unique<RenderPass>();
        basePass->renderTargets = { RT_basePass };
        basePass->depthTex = RT_basePassDepth;

        finalPass = std::make_unique<RenderPass>();
        finalPass->renderTargets = { RT_final };
        finalPass->sourceTextures["mainTex"] = RT_basePass;
        finalPass->sourceTextures["depthTex"] = RT_basePassDepth;
    }
    
    // create actors
    {
        A_quad = std::make_unique<Actor>();
        A_quad->mesh = Mesh::Find("BUILD_IN_QUAD");
        A_quad->material = new Material();
        A_quad->material->shader = Shader::Find("Shaders/blit_test.hlsl");
    }

    // init scene
    scene = std::make_unique<Scene>();

    // init editor
    {
        editor = std::make_unique<Editor>();

        editor->depthTex = RT_basePassDepth;
        editor->RT_final = RT_final;

        editor->scene = scene.get();

        editor->Init(g_width, g_height, g_hwnd);
    }

}

void Engine::OnUpdate()
{
    double delta_time = timer.End();
    Tick(delta_time);
    timer.Start();
}

void Engine::OnRender()
{
	GraphicContex::PreRender();
	
    // first pass
    auto actors = scene->GetRenderObjects();
    GraphicContex::RenderLoop(camera, basePass.get(), actors);
    
    // second pass
    GraphicContex::RenderLoop(camera, finalPass.get(), { A_quad.get() });
    
    // UI pass
    editor->PreGUI();
    editor->RenderGUI();
    editor->PostGUI();

	GraphicContex::PostRender();
}

void Engine::OnDestroy()
{
    // aotu save
    scene->SaveToFile();

    // release galobal resource
    RenderTexture::FreeAll();
    DepthTexture::FreeAll();
    Texture2D::FreeAll();
    Shader::FreeAll();
    Material::FreeAll();
    Mesh::FreeAll();

    // release pipeline
	GraphicContex::Destory();
}

void Engine::Tick(double delta_time)
{
    
}