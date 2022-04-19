#include "Engine.h"

Engine::Engine()
{
    m_frameIndex = 0;
    m_viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(Win32App::m_width), static_cast<float>(Win32App::m_height));
    m_scissorRect = CD3DX12_RECT(0, 0, static_cast<LONG>(Win32App::m_width), static_cast<LONG>(Win32App::m_height));
    //m_rtvDescriptorSize = 0;
}

Engine::~Engine()
{

}

void Engine::OnInit()
{
    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();

            // Enable additional debug layers.
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    ComPtr<IDXGIFactory4> factory;
    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

    /*
    if (m_useWarpDevice)
    {
        ComPtr<IDXGIAdapter> warpAdapter;
        ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

        ThrowIfFailed(D3D12CreateDevice(
            warpAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&m_device)
        ));
    }
    else*/
    {
        ComPtr<IDXGIAdapter1> hardwareAdapter;
        GetHardwareAdapter(factory.Get(), &hardwareAdapter);

        ThrowIfFailed(D3D12CreateDevice(
            hardwareAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&m_device)
        ));
    }

    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

    // Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.Width = Win32App::m_width;
    swapChainDesc.Height = Win32App::m_height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swapChain;
    ThrowIfFailed(factory->CreateSwapChainForHwnd(
        m_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
        Win32App::m_hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain
    ));

    // This sample does not support fullscreen transitions.
    ThrowIfFailed(factory->MakeWindowAssociation(Win32App::m_hwnd, DXGI_MWA_NO_ALT_ENTER));

    ThrowIfFailed(swapChain.As(&m_swapChain));
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();



    // Create descriptor heaps.
    m_rtvHeap = std::make_shared<DescriptorHeap>(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
    m_resourceHeap = std::make_shared<DescriptorHeap>(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
    m_samplerHeap = std::make_shared<DescriptorHeap>(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);



    // Create frame resources.
    {
        // Create a RTV and a command allocator for each frame.
        for (UINT i = 0; i < FrameCount; i++)
        {
            // alloc a descriptor
            UINT id = m_rtvHeap->AllocDescriptor(); // index in heap
            m_rtvHandles[i] = m_rtvHeap->GetCpuHandle(id);

            ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])));
            m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, m_rtvHandles[i]);

            ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[i])));
        }
    }


    
    // Create root signature.
    {
        // 2D table to hold different types of descriptor
        CD3DX12_DESCRIPTOR_RANGE ranges[3];
   
        ranges[0].Init(
            D3D12_DESCRIPTOR_RANGE_TYPE_SRV,                // for srv descriptor
            1,                       // Number of descriptors in table
            0);                                             // bind to register t0
 
        ranges[1].Init(
            D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,            // for sampler descriptor
            1,
            0);                                             // bind to register s0           

        ranges[2].Init(
            D3D12_DESCRIPTOR_RANGE_TYPE_CBV,                // for cbv descriptor
            1,
            0);                                             // bind to register b0      

        // Root parameter can be a table, root descriptor or root constants.
        CD3DX12_ROOT_PARAMETER rootParameters[3];
        rootParameters[0].InitAsDescriptorTable(1, &ranges[0]);
        rootParameters[1].InitAsDescriptorTable(1, &ranges[1]);
        rootParameters[2].InitAsDescriptorTable(1, &ranges[2]);

        // A root signature is an array of root parameters.
        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init(3, rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
        ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
    }
    


    // Create the pipeline state, which includes compiling and loading shaders.
    {
        std::unique_ptr<Shader> pShader(new Shader(L"D:/PhoenixRoost/src/Shaders/shaders.hlsl"));

        // Define the vertex input layout.
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        // Describe and create the graphics pipeline state object (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = m_rootSignature.Get();
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(pShader->GetVertexShader());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(pShader->GetPixelShader());
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;
        ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
    }



    // Create the command list.
    ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[m_frameIndex].Get(), NULL, IID_PPV_ARGS(&m_commandList)));

    // Command lists are created in the recording state, but there is nothing
    // to record yet. The main loop expects it to be closed, so close it now.
    ThrowIfFailed(m_commandList->Close());


   
    // Create the vertex buffer.
    {
        struct Vertex
        {
            Vertex(float x, float y, float z, float u, float v) : position(x, y, z), texcoord(u, v) {}
            XMFLOAT3 position;
            XMFLOAT2 texcoord;
        };

        // Define the geometry for a triangle.
        std::vector<Vertex> triangleVertices =
        {
            // front face
            { -0.5f,  0.5f, -0.5f, 0.0f, 1.0f },
            {  0.5f, -0.5f, -0.5f, 1.0f, 0.0f },
            { -0.5f, -0.5f, -0.5f, 0.0f, 0.0f },
            {  0.5f,  0.5f, -0.5f, 1.0f, 1.0f },

            // right side face
            {  0.5f, -0.5f, -0.5f, 0.0f, 0.0f },
            {  0.5f,  0.5f,  0.5f, 1.0f, 1.0f },
            {  0.5f, -0.5f,  0.5f, 1.0f, 0.0f },
            {  0.5f,  0.5f, -0.5f, 0.0f, 1.0f },

            // left side face
            { -0.5f,  0.5f,  0.5f, 0.0f, 1.0f },
            { -0.5f, -0.5f, -0.5f, 1.0f, 0.0f },
            { -0.5f, -0.5f,  0.5f, 0.0f, 0.0f },
            { -0.5f,  0.5f, -0.5f, 1.0f, 1.0f },

            // back face
            {  0.5f,  0.5f,  0.5f, 0.0f, 1.0f },
            { -0.5f, -0.5f,  0.5f, 1.0f, 0.0f },
            {  0.5f, -0.5f,  0.5f, 0.0f, 0.0f },
            { -0.5f,  0.5f,  0.5f, 1.0f, 1.0f },

            // top face
            { -0.5f,  0.5f, -0.5f, 0.0f, 0.0f },
            {  0.5f,  0.5f,  0.5f, 1.0f, 1.0f },
            {  0.5f,  0.5f, -0.5f, 1.0f, 0.0f },
            { -0.5f,  0.5f,  0.5f, 0.0f, 1.0f },

            // bottom face
            {  0.5f, -0.5f,  0.5f, 0.0f, 1.0f },
            { -0.5f, -0.5f, -0.5f, 1.0f, 0.0f },
            {  0.5f, -0.5f, -0.5f, 0.0f, 0.0f },
            { -0.5f, -0.5f,  0.5f, 1.0f, 1.0f },
        };

        std::vector<UINT> triangleIndices =
        {
            // ffront face
            0, 1, 2, // first triangle
            0, 3, 1, // second triangle

            // left face
            4, 5, 6, // first triangle
            4, 7, 5, // second triangle

            // right face
            8, 9, 10, // first triangle
            8, 11, 9, // second triangle

            // back face
            12, 13, 14, // first triangle
            12, 15, 13, // second triangle

            // top face
            16, 17, 18, // first triangle
            16, 19, 17, // second triangle

            // bottom face
            20, 21, 22, // first triangle
            20, 23, 21, // second triangle
        };

        m_vertexBuffer = std::make_shared<VertexBuffer>(m_device.Get(), L"vertBuffer");
        m_vertexBuffer->UploadVertexData(triangleVertices);

        m_indexBuffer = std::make_shared<IndexBuffer>(m_device.Get(), L"idxBuffer");
        m_indexBuffer->UploadIndexData(triangleIndices);
    }

    // create tex
    m_texture2D = std::make_shared<Texture2D>(m_device.Get(), m_resourceHeap.get(), m_samplerHeap.get(), "D:/PhoenixRoost/asset/93632004_p0.png");

    // create const buffer for matrix
    m_constBuffer = std::make_shared<ConstBuffer>(m_device.Get(), m_resourceHeap.get());

    // Create synchronization objects and wait until assets have been uploaded to the GPU.
    {
        ThrowIfFailed(m_device->CreateFence(m_fenceValues[m_frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
        m_fenceValues[m_frameIndex]++;

        // Create an event handle to use for frame synchronization.
        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (m_fenceEvent == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }

        // Wait for the command list to execute; we are reusing the same command 
        // list in our main loop but for now, we just want to wait for setup to 
        // complete before continuing.
        WaitForGpu();
    }
}

void Engine::OnUpdate()
{
    // i : matrix must transport before send to GPU
    XMMATRIX m, v, p;

    // model matrix
    m_rotateAngle += 0.01f;
    m = XMMatrixRotationY(m_rotateAngle);
    XMStoreFloat4x4(&m_modelMatrix, XMMatrixTranspose(m));
    
    // build view matrix
    XMVECTOR cPos = XMLoadFloat4(&m_cameraPosition);
    XMVECTOR cTarg = XMLoadFloat4(&m_cameraTarget);
    XMVECTOR cUp = XMLoadFloat4(&m_cameraUp);
    v = XMMatrixLookAtLH(cPos, cTarg, cUp);
    XMStoreFloat4x4(&m_viewMatrix, XMMatrixTranspose(v));

    // build projection matrix
    p = XMMatrixPerspectiveFovLH(60.0f * (3.14f / 180.0f), (float)Win32App::m_width / (float)Win32App::m_height, 0.1f, 1000.0f);
    XMStoreFloat4x4(&m_projectionMatrix, XMMatrixTranspose(p));

    // mvp
    XMFLOAT4X4 mvpMatrix;
    XMStoreFloat4x4(&mvpMatrix, XMMatrixTranspose(m * v * p));

    XMFLOAT4X4 mats[4] = {m_modelMatrix, m_viewMatrix, m_projectionMatrix, mvpMatrix };
    m_constBuffer->UpdateData(mats, sizeof(mats));
}

void Engine::OnRender()
{
    // Command list allocators can only be reset when the associated 
    // command lists have finished execution on the GPU; apps should use 
    // fences to determine GPU execution progress.
    ThrowIfFailed(m_commandAllocators[m_frameIndex]->Reset());

    // However, when ExecuteCommandList() is called on a particular command 
    // list, that command list can then be reset at any time and must be before 
    // re-recording.
    ThrowIfFailed(m_commandList->Reset(m_commandAllocators[m_frameIndex].Get(), m_pipelineState.Get()));



    // Set necessary state.
    m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());

    // set the descriptor heap
    ID3D12DescriptorHeap* ppHeaps[] = { m_resourceHeap->heap.Get(), m_samplerHeap->heap.Get() };
    m_commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
    
    // set srv descriptor table, 0 is for index in root parameter table
    // in my code, rootParameter[0] is for srv descriptors, while rootParameter[1] is for sampler descriptors, rootParameter[2] is for const buffer descriptor
    m_commandList->SetGraphicsRootDescriptorTable(0, m_texture2D->srvGpuHandle);
    m_commandList->SetGraphicsRootDescriptorTable(1, m_texture2D->samplerGpuHandle);
    m_commandList->SetGraphicsRootDescriptorTable(2, m_constBuffer->cbvGpuHandle);

    m_commandList->RSSetViewports(1, &m_viewport);
    m_commandList->RSSetScissorRects(1, &m_scissorRect);



    // Indicate that the back buffer will be used as a render target.
    auto barrierRT = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_commandList->ResourceBarrier(1, &barrierRT);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHandles[m_frameIndex];
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);



    // Record commands.
    const float clearColor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_commandList->IASetVertexBuffers(0, 1, &m_vertexBuffer->bufferView);
    m_commandList->IASetIndexBuffer(&m_indexBuffer->bufferView);
    m_commandList->DrawIndexedInstanced(m_indexBuffer->indexCount, 1, 0, 0, 0);



    // Indicate that the back buffer will now be used to present.
    auto barrierPr = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_commandList->ResourceBarrier(1, &barrierPr);

    ThrowIfFailed(m_commandList->Close());



    // Execute the command list.
    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Present the frame.
    ThrowIfFailed(m_swapChain->Present(1, 0));

    MoveToNextFrame();
}

void Engine::OnDestroy()
{
    // Ensure that the GPU is no longer referencing resources that are about to be
    // cleaned up by the destructor.
    WaitForGpu();

    CloseHandle(m_fenceEvent);
}


// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
// If no such adapter can be found, *ppAdapter will be set to nullptr.
_Use_decl_annotations_
void Engine::GetHardwareAdapter(
    IDXGIFactory1* pFactory,
    IDXGIAdapter1** ppAdapter,
    bool requestHighPerformanceAdapter)
{
    *ppAdapter = nullptr;

    ComPtr<IDXGIAdapter1> adapter;

    ComPtr<IDXGIFactory6> factory6;
    if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
    {
        for (
            UINT adapterIndex = 0;
            SUCCEEDED(factory6->EnumAdapterByGpuPreference(
                adapterIndex,
                requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
                IID_PPV_ARGS(&adapter)));
            ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // Don't select the Basic Render Driver adapter.
                // If you want a software adapter, pass in "/warp" on the command line.
                continue;
            }

            // Check to see whether the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }

    if (adapter.Get() == nullptr)
    {
        for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // Don't select the Basic Render Driver adapter.
                // If you want a software adapter, pass in "/warp" on the command line.
                continue;
            }

            // Check to see whether the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }

    *ppAdapter = adapter.Detach();
}

void Engine::WaitForGpu()
{
    // Schedule a Signal command in the queue.
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_fenceValues[m_frameIndex]));

    // Wait until the fence has been processed.
    ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
    WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

    // Increment the fence value for the current frame.
    m_fenceValues[m_frameIndex]++;
}

void Engine::MoveToNextFrame()
{
    // Schedule a Signal command in the queue.
    const UINT64 currentFenceValue = m_fenceValues[m_frameIndex];
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), currentFenceValue));

    // Update the frame index.
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    // If the next frame is not ready to be rendered yet, wait until it is ready.
    if (m_fence->GetCompletedValue() < m_fenceValues[m_frameIndex])
    {
        ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
        WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
    }

    // Set the fence value for the next frame.
    m_fenceValues[m_frameIndex] = currentFenceValue + 1;
}
