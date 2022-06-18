#include "Editor.h"

#include "../Library/imgui/imgui.h"
#include "../Library/imgui/imgui_impl_win32.h"
#include "../Library/imgui/imgui_impl_dx12.h"

#include "../Core/helper.h"
#include "../Core/GraphicContex.h"

Editor::Editor()
{

}

Editor::~Editor()
{
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void Editor::Init(int w, int h, HWND hwnd)
{
    g_width = w;
    g_height = h;

    // use our global heap
    // GraphicContex::g_srvHeap
    /*
    // create heap
    D3D12_DESCRIPTOR_HEAP_DESC SrvHeapDesc;
    SrvHeapDesc.NumDescriptors = 1;
    SrvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    SrvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    SrvHeapDesc.NodeMask = 0;
    ThrowIfFailed(GraphicContex::g_device->CreateDescriptorHeap(
        &SrvHeapDesc, IID_PPV_ARGS(mSrvHeap.GetAddressOf())));
    */

    // alloc descriptor from global heap
    srvHandleIndex = GraphicContex::g_srvHeap->AllocDescriptor();
    srvCpuHandle = GraphicContex::g_srvHeap->GetCpuHandle(srvHandleIndex);
    srvGpuHandle = GraphicContex::g_srvHeap->GetGpuHandle(srvHandleIndex);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX12_Init(GraphicContex::g_device.Get(), GraphicContex::FrameCount,
        DXGI_FORMAT_R8G8B8A8_UNORM, 
        GraphicContex::g_srvHeap->heap.Get(),
        srvCpuHandle,
        srvGpuHandle
    );

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    ImFont* font = io.Fonts->AddFontFromFileTTF("Library/imgui/fonts/Cousine-Regular.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    IM_ASSERT(font != NULL);

    // 
    emptyRenderPass = std::make_unique<RenderPass>();

    // load scene
    scene->LoadFromFile("Asset/roost_scene.json");
}

void Editor::PreGUI()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void Editor::PostGUI()
{
    // set render target to screen use a empty "RenderPass" class
    GraphicContex::SetRenderTarget(emptyRenderPass.get());
    GraphicContex::ClearRenderTarget(emptyRenderPass.get(), XMFLOAT3(0.5, 0.5, 0.5));

    //GraphicContex::g_commandList->SetDescriptorHeaps(1, mSrvHeap.GetAddressOf());
    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), GraphicContex::g_commandList.Get());
}

void Editor::RenderGUI()
{
    //
    int corner = 0; // left top
    const float PAD = 0.0f;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiWindowFlags window_flags = 0;
    ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
    ImVec2 work_size = viewport->WorkSize;
    ImVec2 window_pos, window_pos_pivot;
    ImVec2 pos;

    window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
    window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
    window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
    window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;

    

    // scene window
    {
        window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoCollapse;
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoResize;

        pos.x = work_pos.x;
        pos.y = work_pos.y;
        ImGui::SetNextWindowPos(pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowSize(ImVec2(g_width * 0.15, g_height * 0.7));

        ImGui::Begin("Scene", 0, window_flags);

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

        ImGui::End();
    }

    // Detail window
    {
        window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoCollapse;
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoResize;

        pos.x = work_pos.x + g_width * 0.85;
        pos.y = work_pos.y;
        ImGui::SetNextWindowPos(pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowSize(ImVec2(g_width * 0.15, g_height * 0.7));

        ImGui::Begin("Detail Information", 0, window_flags);

        ImGui::ShowDemoWindow();

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

        ImGui::End();
    }
   
    // show depth
    if (show_depth_tex)
    {
        ImGui::Begin("DirectX12 Texture Test");
        ImGui::Text("CPU handle = %p", depthTex->srvCpuHandle);
        ImGui::Text("GPU handle = %p", depthTex->srvGpuHandle);
        ImGui::Text("size = %d x %d", depthTex->width, depthTex->height);
        float w = depthTex->width / 3.0f;
        float h = depthTex->height / 3.0f;

        auto hdptr = D3D12_GPU_DESCRIPTOR_HANDLE(depthTex->srvGpuHandle).ptr;

        // Note that we pass the GPU SRV handle here, *not* the CPU handle. We're passing the internal pointer value, cast to an ImTextureID
        ImGui::Image((ImTextureID)hdptr, ImVec2(w, h));
        ImGui::End();
    }

    // show screen buffer
    {
        window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoCollapse;
        window_flags |= ImGuiWindowFlags_NoScrollbar;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

        pos.x = work_pos.x + g_width * 0.15;
        pos.y = work_pos.y;
        ImGui::SetNextWindowPos(pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowSize(ImVec2(g_width * 0.7, g_height * 0.7));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

        ImGui::Begin("screen", 0, window_flags);
        //ImGui::Text("size = %d x %d", depthTex->width, depthTex->height);

        auto hdptr = D3D12_GPU_DESCRIPTOR_HANDLE(RT_final->srvGpuHandle).ptr;
        ImGui::Image((ImTextureID)hdptr, ImVec2(g_width * 0.7, g_height * 0.7));
        ImGui::End();
        ImGui::PopStyleVar();
    }
    
    // show frame rate
    {
        window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoCollapse;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
        window_flags |= ImGuiWindowFlags_NoTitleBar;

        ImGui::SetNextWindowBgAlpha(0.35f);
        pos.x = work_pos.x + g_width * 0.15;
        pos.y = work_pos.y;
        ImGui::SetNextWindowPos(pos, ImGuiCond_Always, window_pos_pivot);

        ImGui::Begin("Profile", 0, window_flags);
        ImGui::Text("Cost %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
        ImGui::Text("FPS  %.1f", ImGui::GetIO().Framerate);
        ImGui::End();
    }
}
