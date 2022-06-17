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
std::unique_ptr<Mesh> quad;

std::unique_ptr<RenderTexture> RT_basePass;
std::unique_ptr<DepthTexture> RT_basePassDepth;

std::unique_ptr<RenderPass> basePass;
std::unique_ptr<RenderPass> finalPass;

std::unique_ptr<Actor> A_spaceship;
std::unique_ptr<Actor> A_cube;

std::unique_ptr<Editor> editor;

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


    // create custom pass
    basePass = std::make_unique<RenderPass>();
    basePass->name = "BasePass";
    basePass->renderTargets = { RT_basePass.get() };
    basePass->depthTex = RT_basePassDepth.get();

    finalPass = std::make_unique<RenderPass>();

    // create actors
    {
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
    }
    

    quad = std::make_unique<Mesh>();
    quad->GenerateQuad();


    // init editor
    editor = std::make_unique<Editor>();
    editor->Init(g_hwnd);

    // for test
    editor->depthTex = RT_basePassDepth.get();
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
    {
        GraphicContex::RenderLoop(camera, basePass.get(), { A_spaceship.get(), A_cube.get() });
    }

    // second pass
    {
        auto blit_shader = Shader::Find("Shaders/blit_test.hlsl");
        auto pso = finalPass->GetPsoByShader(blit_shader);
        GraphicContex::g_commandList->SetPipelineState(pso);

        blit_shader->SetTexture("mainTex", basePass->renderTargets[0]);
        blit_shader->SetTexture("depthTex", basePass->depthTex);
        blit_shader->Activate();

        GraphicContex::SetRenderTarget(finalPass.get());
        GraphicContex::ClearRenderTarget(finalPass.get(), XMFLOAT3(0.5, 0.5, 0.5));

        quad->Draw();
    }
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
    A_spaceship->transform.rotation.y += 45.0f * delta_time;
    A_cube->transform.rotation.y += -90.0f * delta_time;
}