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

void Editor::Init(HWND hwnd)
{
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
}

void Editor::PreGUI()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void Editor::PostGUI()
{
    //GraphicContex::g_commandList->SetDescriptorHeaps(1, mSrvHeap.GetAddressOf());
    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), GraphicContex::g_commandList.Get());
}

void Editor::RenderGUI()
{
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        
    ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);


    ImGui::Checkbox("Another Window", &show_another_window);

    ImGui::Checkbox("Show Depth buffer", &show_depth_tex);

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
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
}
