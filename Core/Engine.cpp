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

std::unique_ptr<RenderTexture> RT_basePass;
std::unique_ptr<DepthTexture> RT_basePassDepth;
std::unique_ptr<RenderTexture> RT_final;

std::unique_ptr<RenderPass> basePass;
std::unique_ptr<RenderPass> finalPass;

/*
std::unique_ptr<Actor> A_spaceship;
std::unique_ptr<Actor> A_cube;*/
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
    RT_basePass = std::make_unique<RenderTexture>(g_width, g_height, DXGI_FORMAT_R32G32B32A32_FLOAT);
    RT_basePassDepth = std::make_unique<DepthTexture>(g_width, g_height);
    RT_final = std::make_unique<RenderTexture>(g_width, g_height, DXGI_FORMAT_R8G8B8A8_UNORM);

    // create custom pass
    {
        basePass = std::make_unique<RenderPass>();
        basePass->renderTargets = { RT_basePass.get() };
        basePass->depthTex = RT_basePassDepth.get();

        finalPass = std::make_unique<RenderPass>();
        finalPass->renderTargets = { RT_final.get() };
        finalPass->sourceTextures["mainTex"] = RT_basePass.get();
        finalPass->sourceTextures["depthTex"] = RT_basePassDepth.get();
    }
    
    // create actors
    {
        /*
        A_spaceship = std::make_unique<Actor>();
        A_spaceship->mesh = Mesh::Find("Asset/spaceship/StarSparrow01.obj");
        A_spaceship->material = new Material();
        A_spaceship->material->shader = Shader::Find("Shaders/unlit.hlsl");
        A_spaceship->material->SetTexture("mainTex", Texture2D::Find("Asset/spaceship/StarSparrow_Red.png"));
        A_spaceship->transform.scale = XMFLOAT3(0.4, 0.4, 0.4);

        A_cube = std::make_unique<Actor>();
        A_cube->mesh = Mesh::Find("Asset/mcube/mcube.obj");
        A_cube->material = new Material();
        A_cube->material->shader = Shader::Find("Shaders/unlit.hlsl");
        A_cube->material->SetTexture("mainTex", Texture2D::Find("Asset/mcube/mcube.png"));
        */

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

        editor->depthTex = RT_basePassDepth.get();
        editor->RT_final = RT_final.get();

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
	GraphicContex::Destory();
}

void Engine::Tick(double delta_time)
{
    
}