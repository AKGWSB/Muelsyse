#include "Editor.h"

#include "../Core/helper.h"
#include "../Core/GraphicContex.h"

#include "ResourceViewer.h"
#include "../Core/Timer.h"

Editor::Editor()
{

}

Editor::~Editor()
{

}

void Editor::Destory()
{
    // aotu save
    scene->SaveToFile();
    graphicEditor->SaveToFile();

    // free resource snap view
    ResourceViewer::ReleaseView();

    // free imgui
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

    font_small= io.Fonts->AddFontFromFileTTF("Library/imgui/fonts/Cousine-Regular.ttf", 11.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    IM_ASSERT(font_small != NULL);

    // 
    emptyRenderPass = std::make_unique<RenderPass>();

    // 
    ResourceViewer::Init();

    //
    graphicEditor = std::make_unique<GraphEditor>();
    graphicEditor->LoadFromFile("Asset/e_pipeline.pipeline");
}

void Editor::PreGUI()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
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

void Editor::RenderDetailPanel()
{
    if (currencSelectedActor)
    {
        if (ImGui::TreeNodeEx("Name", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text(currencSelectedActor->name.c_str());
            ImGui::TreePop();
        }

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // select manipulate mode
            if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
            {
                mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
                mCurrentGizmoMode = ImGuizmo::WORLD;
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
            {
                mCurrentGizmoOperation = ImGuizmo::ROTATE;
                mCurrentGizmoMode = ImGuizmo::LOCAL;
            }
                
            ImGui::SameLine();
            if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
            {
                mCurrentGizmoOperation = ImGuizmo::SCALE;
                mCurrentGizmoMode = ImGuizmo::LOCAL;
            }

            Transform& transform = currencSelectedActor->transform;
            ImGui::InputFloat3("position", (float*)&transform.position);
            ImGui::InputFloat3("rotation", (float*)&transform.rotation);
            ImGui::InputFloat3("scale", (float*)&transform.scale);

            ImGui::TreePop();
        }

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        if (ImGui::TreeNodeEx("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // open resource viewer
            auto& name = currencSelectedActor->mesh->name;
            ImGui::Text(name.c_str());

            Texture2D* view = ResourceViewer::GetResourceViewByName(name, ResourceViewerOpenMode::EMesh);
            auto hdptr = D3D12_GPU_DESCRIPTOR_HANDLE(view->srvGpuHandle).ptr;
            ImGui::Image((ImTextureID)hdptr, ImVec2(128, 128), ImVec2(0, 0), ImVec2(1, 1));

            std::string buttonName = "Select Mesh";
            if (ImGui::Button(buttonName.c_str()))
            {
                ResourceViewer::Open(buttonName, ResourceViewerOpenMode::EMesh);
            }

            // change mesh
            std::string filepath;
            if (ResourceViewer::GetSelectResourceName(filepath, buttonName))
            {
                currencSelectedActor->mesh = Mesh::Find(filepath);
            }

            ImGui::TreePop();
        }

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_DefaultOpen))
        {
            auto& name = currencSelectedActor->material->name;
            ImGui::Text(name.c_str());

            Texture2D* view = ResourceViewer::GetResourceViewByName(name, ResourceViewerOpenMode::EMaterial);
            auto hdptr = D3D12_GPU_DESCRIPTOR_HANDLE(view->srvGpuHandle).ptr;
            ImGui::Image((ImTextureID)hdptr, ImVec2(128, 128), ImVec2(0, 0), ImVec2(1, 1));

            std::string buttonName = "Select Material";
            if (ImGui::Button(buttonName.c_str()))
            {
                ResourceViewer::Open(buttonName, ResourceViewerOpenMode::EMaterial);
            }
            // change mat
            std::string filepath;
            if (ResourceViewer::GetSelectResourceName(filepath, buttonName))
            {
                currencSelectedActor->material = Material::Find(filepath);
            }


            ImGui::Dummy(ImVec2(0.0f, 10.0f));
            if (ImGui::TreeNodeEx("shader", ImGuiTreeNodeFlags_DefaultOpen))
            {
                // open resource viewer
                ImGui::Text(currencSelectedActor->material->shader->name.c_str());
                std::string buttonName = "Select Shader";
                if (ImGui::Button(buttonName.c_str()))
                {
                    ResourceViewer::Open(buttonName, ResourceViewerOpenMode::EShader);
                }

                // change tex
                std::string filepath;
                if (ResourceViewer::GetSelectResourceName(filepath, buttonName))
                {
                    currencSelectedActor->material->shader = Shader::Find(filepath);
                }

                ImGui::TreePop();
            }

            ImGui::Dummy(ImVec2(0.0f, 10.0f));
            if (ImGui::TreeNodeEx("textures", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::Button("Add Texture"))
                {
                    std::string addTexName = "tempTex_" + std::to_string(rand());
                    currencSelectedActor->material->textures[addTexName] = Texture2D::Find("Core/TEXTURE_NOT_FOUND.png");
                }

                // texture change list
                std::vector<std::string> texDeleteList;

                // varname (in shader) changed from p.first --> p.second
                std::vector<std::pair<std::string, std::string>> varNameChangeList;

                if (ImGui::BeginTable("split", 2, ImGuiTableFlags_Borders))
                {
                    int i = 0;
                    for (auto& p : currencSelectedActor->material->textures)
                    {
                        i++;
                        auto varname = p.first;     // texture name in shader, eg: _mainTex
                        auto tex = p.second;

                        // table left side
                        ImGui::TableNextColumn();
                        // view pic
                        auto hdptr = D3D12_GPU_DESCRIPTOR_HANDLE(tex->srvGpuHandle).ptr;
                        ImGui::Image((ImTextureID)hdptr, ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));

                        // table right side
                        ImGui::TableNextColumn();
                        ImGui::Text("Var Name: ");
                        ImGui::Text(varname.c_str());

                        // texture's varname
                        std::string buttonName = "Change##" + varname;
                        std::string newVarname;
                        if (ImGui::Button(buttonName.c_str()))
                        {
                            ResourceViewer::Open(buttonName, ResourceViewerOpenMode::EInputText);
                        }
                        if (ResourceViewer::GetSelectResourceName(newVarname, buttonName))
                        {
                            // change varname
                            if (currencSelectedActor->material->textures.find(newVarname) == currencSelectedActor->material->textures.end())
                            {
                                varNameChangeList.push_back({ varname , newVarname });
                            }
                        }

                        // open resource viewer
                        ImGui::Dummy(ImVec2(0.0f, 10.0f));
                        ImGui::Text("Operation");
                        buttonName = "Select Texture##" + varname;
                        if (ImGui::Button(buttonName.c_str()))
                        {
                            ResourceViewer::Open(buttonName, ResourceViewerOpenMode::ETexture2D);
                        }
                        // remove button
                        std::string delButtonName = "Remove Texture##" + varname;
                        if (ImGui::Button(delButtonName.c_str()))
                        {
                            texDeleteList.push_back(varname);
                        }
                        ImGui::Dummy(ImVec2(0.0f, 10.0f));

                        // change tex
                        std::string filepath;
                        if (ResourceViewer::GetSelectResourceName(filepath, buttonName))
                        {
                            currencSelectedActor->material->SetTexture(varname, Texture2D::Find(filepath));
                        }
                    }
                    ImGui::EndTable();
                }

                // delete texture
                for (auto& name : texDeleteList)
                {
                    currencSelectedActor->material->textures.erase(name);
                }

                // add texture

                // change material varname
                for (auto& p : varNameChangeList)
                {
                    auto& oldVarName = p.first;
                    auto& newVarName = p.second;

                    currencSelectedActor->material->textures[newVarName] = currencSelectedActor->material->textures[oldVarName];
                    currencSelectedActor->material->textures.erase(oldVarName);
                }

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }
    else
    {
        ImGui::Text("Select an Actor to view detail");
    }
}

void Editor::RenderScenePanel()
{
    if (ImGui::Button("New"))
    {

    }
    ImGui::SameLine();
    if (ImGui::Button("Load"))
    {

    }
    ImGui::SameLine();
    if (ImGui::Button("Save"))
    {
        scene->SaveToFile();
    }
    

    // name
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    if (ImGui::TreeNodeEx("Name", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text(scene->name.c_str());
        ImGui::TreePop();
    }

    // cameras
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    if (ImGui::TreeNodeEx("Camera list", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("mainCamera");
        ImGui::TreePop();
    }

    // actors
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    if (ImGui::TreeNodeEx("Actors list", ImGuiTreeNodeFlags_DefaultOpen))
    {
        std::vector<std::string> items;

        // collect names
        for (auto& actor : scene->actors)
        {
            items.push_back(actor->name);
        }

        if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
        {
            for (int i = 0; i < items.size(); i++)
            {
                const bool is_selected = (actor_item_current_idx == i);
                if (ImGui::Selectable(items[i].c_str(), is_selected)) {
                    actor_item_current_idx = i;
                }
                // set choosen actor
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                    currencSelectedActor = scene->actors[actor_item_current_idx].get();
                }
            }
            ImGui::EndListBox();
        }

        ImGui::TreePop();
    }
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
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoResize;

        pos.x = work_pos.x;
        pos.y = work_pos.y;
        ImGui::SetNextWindowPos(pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowSize(ImVec2(g_width * 0.16, g_height));

        ImGui::Begin("Scene", 0, window_flags);

        RenderScenePanel();

        ImGui::End();
    }

    // Detail window
    {
        window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoResize;

        pos.x = work_pos.x + g_width * 0.7;
        pos.y = work_pos.y;
        ImGui::SetNextWindowPos(pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowSize(ImVec2(g_width * 0.3, g_height));

        ImGui::Begin("Detail Information", 0, window_flags);

        RenderDetailPanel();

        ImGui::End();
    }

    // show screen buffer
    {
        /**/
        window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoCollapse;
        window_flags |= ImGuiWindowFlags_NoScrollbar;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

        pos.x = work_pos.x;
        pos.y = work_pos.y;
        ImGui::SetNextWindowPos(pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowSize(ImVec2(g_width, g_height));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

        ImGui::Begin("screen", 0, window_flags);
        //ImGui::Text("size = %d x %d", depthTex->width, depthTex->height);

        auto hdptr = D3D12_GPU_DESCRIPTOR_HANDLE(RT_final->srvGpuHandle).ptr;
        ImGui::Image((ImTextureID)hdptr, ImVec2(g_width, g_height));
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
        pos.x = work_pos.x + g_width * 0.16;
        pos.y = work_pos.y + 32;
        ImGui::SetNextWindowPos(pos, ImGuiCond_Always, window_pos_pivot);

        ImGui::Begin("Profile", 0, window_flags);
        ImGui::Text("Cost %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
        ImGui::Text("FPS  %.1f", ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // show guizmo
    {
        Transform& transform = currencSelectedActor->transform;
        Camera& mainCamera = scene->cameraPool["mainCamera"];
        
        // xm to float array
        XMMATRIX _modelMatrix = XMMatrixTranspose(transform.GetTransformMatrix());
        XMMATRIX _viewMatrix = XMMatrixTranspose(mainCamera.GetViewMatrix());
        XMMATRIX _projectionMatrix = XMMatrixTranspose(mainCamera.GetProjectionMatrix());

        XMFLOAT4X4 modelMatrix, viewMatrix, projectionMatrix;
        XMStoreFloat4x4(&modelMatrix, _modelMatrix);
        XMStoreFloat4x4(&viewMatrix, _viewMatrix);
        XMStoreFloat4x4(&projectionMatrix, _projectionMatrix);

        ImGuiIO& io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        ImGuizmo::Manipulate(
            (const float*)&viewMatrix, (const float*)&projectionMatrix, 
            mCurrentGizmoOperation, mCurrentGizmoMode, 
            (float*)&modelMatrix, NULL, NULL
        );

        ImGuizmo::DecomposeMatrixToComponents((const float*)&modelMatrix, (float*)&transform.position, (float*)&transform.rotation, (float*)&transform.scale);
    }

    // for debug
    //ImGui::ShowDemoWindow();

    // show viewer
    ResourceViewer::RenderUI();
    
    // show frame graph
    graphicEditor->RenderUI();

    // handle input
    double delta_time = 1.0f / 60.0f;

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left | ImGuiMouseButton_Left, true))
    {

    }

    float moveFront = 0.0f;
    float moveRight = 0.0f;
    if (ImGui::IsKeyDown(ImGuiKey_W))
    {
        moveFront += 1.0f;
    }
    if (ImGui::IsKeyDown(ImGuiKey_S))
    {
        moveFront -= 1.0f;
    }
    if (ImGui::IsKeyDown(ImGuiKey_A))
    {
        moveRight -= 1.0f;
    }
    if (ImGui::IsKeyDown(ImGuiKey_D))
    {
        moveRight += 1.0f;
    }

    Camera& mainCamera = scene->cameraPool["mainCamera"];
    XMFLOAT3 cameraPosition = mainCamera.GetPosition();
    cameraPosition.x += moveRight * 5.0f * delta_time;
    cameraPosition.z += moveFront * 5.0f * delta_time;
    mainCamera.SetPosition(cameraPosition);
}

void Editor::Render()
{
    // render each pass, "compiler" from "GraphEditor"
    ResourceViewer::RenderResourceView();

    // UI pass
    this->PreGUI();
    this->RenderGUI();
    this->PostGUI();
}
