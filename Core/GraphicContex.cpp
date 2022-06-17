#include "GraphicContex.h"
#include "helper.h"
#include "d3dx12.h"
#include "../Rendering/Transform.h"

// ---------------------------------------------------------------------- //

int GraphicContex::screenWidth;
int GraphicContex::screenHeight;

// Pipeline objects.
ComPtr<IDXGISwapChain3> GraphicContex::g_swapChain;
ComPtr<ID3D12Device> GraphicContex::g_device;

// frame resource
UINT GraphicContex::m_rtvIndices[GraphicContex::FrameCount];
ComPtr<ID3D12Resource> GraphicContex::m_renderTargets[GraphicContex::FrameCount];

ComPtr<ID3D12CommandAllocator> GraphicContex::g_commandAllocator;
ComPtr<ID3D12CommandQueue> GraphicContex::g_commandQueue;
ComPtr<ID3D12GraphicsCommandList> GraphicContex::g_commandList;

// descriptor heaps
std::unique_ptr<DescriptorHeap> GraphicContex::g_rtvHeap;
std::unique_ptr<DescriptorHeap> GraphicContex::g_srvHeap;
std::unique_ptr<DescriptorHeap> GraphicContex::g_splHeap;
std::unique_ptr<DescriptorHeap> GraphicContex::g_dsvHeap;

// Synchronization objects.
UINT GraphicContex::m_frameIndex;
HANDLE GraphicContex::m_fenceEvent;
ComPtr<ID3D12Fence> GraphicContex::m_fence;
UINT64 GraphicContex::m_fenceValue;

// ---------------------------------------------------------------------- //

void GraphicContex::Init(int width, int height, HWND hwnd)
{
    screenWidth = width;
    screenHeight = height;

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

    ComPtr<IDXGIAdapter1> hardwareAdapter;
    GetHardwareAdapter(factory.Get(), &hardwareAdapter, true);
    ThrowIfFailed(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&g_device)));

    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ThrowIfFailed(g_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&g_commandQueue)));

    // Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swapChain;
    ThrowIfFailed(factory->CreateSwapChainForHwnd(
        g_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
        hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain
    ));

    // This sample does not support fullscreen transitions.
    ThrowIfFailed(factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));

    ThrowIfFailed(swapChain.As(&g_swapChain));
    m_frameIndex = g_swapChain->GetCurrentBackBufferIndex();



    // Create descriptor heaps.
    g_rtvHeap = std::make_unique<DescriptorHeap>(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
    g_srvHeap = std::make_unique<DescriptorHeap>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
    g_splHeap = std::make_unique<DescriptorHeap>(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
    g_dsvHeap = std::make_unique<DescriptorHeap>(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
    
    

    // Create frame resources.
    {
        // Create a RTV for each frame.
        for (UINT i = 0; i < FrameCount; i++)
        {
            UINT rtv_descriptorID = g_rtvHeap->AllocDescriptor();
            m_rtvIndices[i] = rtv_descriptorID;
            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_rtvHeap->GetCpuHandle(rtv_descriptorID);

            ThrowIfFailed(g_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])));
            g_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
        }
    }

    ThrowIfFailed(g_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_commandAllocator)));



    // Create the command list.
    ThrowIfFailed(g_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_commandAllocator.Get(), NULL, IID_PPV_ARGS(&g_commandList)));

    // Command lists are created in the recording state, but there is nothing
    // to record yet. The main loop expects it to be closed, so close it now.
    ThrowIfFailed(g_commandList->Close());

    // Create synchronization objects.
    {
        ThrowIfFailed(g_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
        m_fenceValue = 1;

        // Create an event handle to use for frame synchronization.
        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (m_fenceEvent == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }
    }

    WaitForPreviousFrame();
}

void GraphicContex::PreRender()
{
    // Command list allocators can only be reset when the associated 
    // command lists have finished execution on the GPU; apps should use 
    // fences to determine GPU execution progress.
    ThrowIfFailed(g_commandAllocator->Reset());

    // However, when ExecuteCommandList() is called on a particular command 
    // list, that command list can then be reset at any time and must be before 
    // re-recording.
    ThrowIfFailed(g_commandList->Reset(g_commandAllocator.Get(), NULL));

    // Indicate that the back buffer will be used as a render target.
    auto to_rt = CD3DX12_RESOURCE_BARRIER::Transition(GetScreenRenderTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    g_commandList->ResourceBarrier(1, &to_rt);

    // set the descriptor heap
    ID3D12DescriptorHeap* ppHeaps[] = { g_srvHeap->heap.Get(), g_splHeap->heap.Get() };
    g_commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

void GraphicContex::Render()
{

}

void GraphicContex::PostRender()
{
    // Indicate that the back buffer will now be used to present.
    auto to_screen = CD3DX12_RESOURCE_BARRIER::Transition(GetScreenRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    g_commandList->ResourceBarrier(1, &to_screen);

    ThrowIfFailed(g_commandList->Close());

    // Execute the command list.
    ID3D12CommandList* ppCommandLists[] = { g_commandList.Get() };
    g_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Present the frame.
    ThrowIfFailed(g_swapChain->Present(1, 0));

    WaitForPreviousFrame();
}

void GraphicContex::Destory()
{
    // Ensure that the GPU is no longer referencing resources that are about to be
    // cleaned up by the destructor.
    WaitForPreviousFrame();

    CloseHandle(m_fenceEvent);
}

void GraphicContex::WaitForPreviousFrame()
{
    // WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
    // This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
    // sample illustrates how to use fences for efficient resource usage and to
    // maximize GPU utilization.

    // Signal and increment the fence value.
    const UINT64 fence = m_fenceValue;
    ThrowIfFailed(g_commandQueue->Signal(m_fence.Get(), fence));
    m_fenceValue++;

    // Wait until the previous frame is finished.
    if (m_fence->GetCompletedValue() < fence)
    {
        ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }

    m_frameIndex = g_swapChain->GetCurrentBackBufferIndex();
}

ID3D12Resource* GraphicContex::GetScreenRenderTarget()
{
    return m_renderTargets[m_frameIndex].Get();
}

void GraphicContex::ClearRenderTarget(RenderPass* pass, XMFLOAT3 clearColor)
{
    float cc[4] = { clearColor.x, clearColor.y, clearColor.z, 1.0 };

    // default set to screen
    if (pass->renderTargets.size() == 0)
    {
        UINT rtvID = m_rtvIndices[m_frameIndex];
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_rtvHeap->GetCpuHandle(rtvID);
        g_commandList->ClearRenderTargetView(rtvHandle, cc, 0, nullptr);

        return;
    }

    // custom render target
    for (auto& rt : pass->renderTargets)
    {
        g_commandList->ClearRenderTargetView(rt->rtvCpuHandle, cc, 0, nullptr);
    }
    
    // depth buffer clear
    if (pass->depthTex != NULL)
    {
        g_commandList->ClearDepthStencilView(pass->depthTex->dsvCpuHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    }
}


void GraphicContex::SetViewport(int width, int height)
{
    CD3DX12_VIEWPORT m_viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
    CD3DX12_RECT m_scissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height));
    g_commandList->RSSetViewports(1, &m_viewport);
    g_commandList->RSSetScissorRects(1, &m_scissorRect);
}

void GraphicContex::SetRenderTarget(RenderPass* pass)
{
    // default set to screen
    if (pass->renderTargets.size() == 0)
    {
        // set view port
        SetViewport(screenWidth, screenHeight);

        // set render target
        UINT rtvID = m_rtvIndices[m_frameIndex];
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_rtvHeap->GetCpuHandle(rtvID);
        g_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

        return;
    }

    // custom render target
    RenderTexture* rt0 = pass->renderTargets[0];
    SetViewport(rt0->width, rt0->height);

    std::vector<CD3DX12_CPU_DESCRIPTOR_HANDLE> rtvHandles;
    for (auto& rt : pass->renderTargets)
    {
        rtvHandles.push_back(rt->rtvCpuHandle);
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE* pDsvHandle = NULL;
    if (pass->depthTex != NULL)
    {
        pDsvHandle = &pass->depthTex->dsvCpuHandle;
    }
    g_commandList->OMSetRenderTargets(1, rtvHandles.data(), FALSE, pDsvHandle);
}

void GraphicContex::RenderLoop(Camera* camera, RenderPass* renderPass, std::vector<Actor*> renderObjects)
{
    // change all render texture to "render target" state
    for (auto& rt : renderPass->renderTargets)
    {
        rt->ChangeToRenderTargetState();
    }

    // set and clear screen
    GraphicContex::SetRenderTarget(renderPass);
    GraphicContex::ClearRenderTarget(renderPass, XMFLOAT3(0.5, 0.5, 0.5));

    for (auto& actor : renderObjects)
    {
        Shader* shader = actor->material->shader;

        // set pipeline state for current shader
        GraphicContex::g_commandList->SetPipelineState(renderPass->GetPsoByShader(shader));

        // set camera matrix to shader
        shader->SetCbuffer("cbPrePass", renderPass->cbufferPrePass.get());
        shader->SetMatrix("cbPrePass", "viewMatrix", camera->GetViewMatrix());
        shader->SetMatrix("cbPrePass", "projectionMatrix", camera->GetProjectionMatrix());

        // draw 
        actor->Draw();
    }

    // change all render texture to "shader resource" state
    for (auto& rt : renderPass->renderTargets)
    {
        rt->ChangeToShaderRsourceState();
    }
}

// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
// If no such adapter can be found, *ppAdapter will be set to nullptr.
_Use_decl_annotations_
void GetHardwareAdapter(
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